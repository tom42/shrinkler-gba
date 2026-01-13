// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <array>
#include <elfio/elfio.hpp>
#include <elfio/elfio_dump.hpp>
#include <gsl/gsl>
#include <string>

module shrinkler_gba;

namespace shrinkler_gba
{

namespace
{

table_printer create_table_printer()
{
    table_printer p;
    p.table_indent(2);
    return p;
}

}

std::string get_section_flags(ELFIO::Elf_Xword flags)
{
    using namespace ELFIO;

    struct table_entry
    {
        Elf_Xword flag;
        char character;
    };

    static const table_entry table[] =
    {
        {SHF_WRITE, 'W'},
        {SHF_ALLOC, 'A'},
        {SHF_EXECINSTR, 'X'},
        {SHF_MERGE, 'M'},
        {SHF_STRINGS, 'S'}
    };

    std::string result;

    const auto original_flags = flags;
    for (const auto& entry : table)
    {
        if (flags & entry.flag)
        {
            result += entry.character;
            flags &= ~entry.flag;
        }
    }

    if (flags)
    {
        // Overwrite result, no return statement here.
        // This avoids triggering Clang's -Wnrvo warning when returning result below.
        // Consider switching off this warning if it becomes annoying.
        result = to_hex(original_flags);
    }

    return result;
}

std::string get_segment_flags(ELFIO::Elf_Word flags)
{
    static const std::array table{ "", "X", "W", "WX", "R", "RX", "RW", "RWX" };

    if (flags < table.size())
    {
        return table[flags];
    }

    return to_hex(flags);
}

void display_program_headers(const ELFIO::elfio& reader, const console& console)
{
    if (!console.is_verbose_enabled())
    {
        return;
    }

    ELFIO::Elf_Half nheaders = reader.segments.size();
    if (nheaders == 0)
    {
        console.verbose("File has no program headers");
        return;
    }

    auto printer = create_table_printer();
    printer.add_row({ "Nr", "Type", "Offset", "VirtAddr", "PhysAddr", "FileSiz", "MemSiz", "Align", "Flg" });
    for (ELFIO::Elf_Half i = 0; i < nheaders; ++i)
    {
        const ELFIO::segment& s = *gsl::not_null(reader.segments[i]);
        printer.add_row({
            std::to_string(i),
            ELFIO::dump::str_segment_type(s.get_type()),
            to_hex(s.get_offset(), 6),
            to_hex(s.get_virtual_address(), 8),
            to_hex(s.get_physical_address(), 8),
            to_hex(s.get_file_size(), 5),
            to_hex(s.get_memory_size(), 5),
            to_hex(s.get_align(), 5),
            get_segment_flags(s.get_flags()) });
    }

    console.verbose("Program headers");
    printer.print(console);
}

void display_section_headers(const ELFIO::elfio& reader, const console& console)
{
    if (!console.is_verbose_enabled())
    {
        return;
    }

    ELFIO::Elf_Half nheaders = reader.sections.size();
    if (nheaders == 0)
    {
        console.verbose("File has no section headers");
        return;
    }

    auto printer = create_table_printer();
    printer.add_row({ "Nr", "Name", "Type", "Addr", "Off", "Size", "ES", "Flg", "Lk", "Inf", "Al", "Inc" });
    for (ELFIO::Elf_Half i = 0; i < nheaders; ++i)
    {
        const ELFIO::section& s = *gsl::not_null(reader.sections[i]);
        printer.add_row({
            std::to_string(i),
            s.get_name(),
            ELFIO::dump::str_section_type(s.get_type()),
            to_hex(s.get_address(), 8),
            to_hex(s.get_offset(), 6),
            to_hex(s.get_size(), 6),
            to_hex(s.get_entry_size(), 2),
            get_section_flags(s.get_flags()),
            to_hex(s.get_link(), 2),
            to_hex(s.get_info(), 3),
            to_hex(s.get_addr_align(), 2),
            is_section_included(&s) ? "Y" : "" });
    }

    console.verbose("Section headers");
    printer.print(console);
}

void display_elf_info(const ELFIO::elfio& reader, const console& console)
{
    display_program_headers(reader, console);
    display_section_headers(reader, console);
}

}
