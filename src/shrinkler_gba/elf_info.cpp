// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <array>
#include <elfio/elfio.hpp>
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

std::string get_section_type(ELFIO::Elf_Word type)
{
    using namespace ELFIO;

    // This are only the most common section types, plus some
    // special ones used for ARM. Add missing ones as needed.
    switch (type)
    {
        case SHT_NULL: return "NULL";
        case SHT_PROGBITS: return "PROGBITS";
        case SHT_SYMTAB: return "SYMTAB";
        case SHT_STRTAB: return "STRTAB";
        case SHT_RELA: return "RELA";
        case SHT_HASH: return "HASH";
        case SHT_DYNAMIC: return "DYNAMIC";
        case SHT_NOTE: return "NOTE";
        case SHT_NOBITS: return "NOBITS";
        case SHT_REL: return "REL";
        case SHT_SHLIB: return "SHLIB";
        case SHT_DYNSYM: return "DYNSYM";
        case SHT_INIT_ARRAY: return "INIT_ARRAY";
        case SHT_FINI_ARRAY: return "FINI_ARRAY";
        case SHT_PREINIT_ARRAY: return "PREINIT_ARRAY";
        case SHT_GROUP: return "GROUP";
        case SHT_SYMTAB_SHNDX: return "SYMTAB_SHNDX";
        case 0x70000003: return "ARM_ATTRIBUTES";
        default:
            return to_hex(type, 8);
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

std::string get_segment_type(ELFIO::Elf_Word type)
{
    static const std::array table{ "NULL", "LOAD", "DYNAMIC", "INTERP", "NOTE", "SHLIB", "PHDR", "TLS" };

    if (type < table.size())
    {
        return table[type];
    }

    return to_hex(type, 8);
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
            get_segment_type(s.get_type()),
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
            get_section_type(s.get_type()),
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
