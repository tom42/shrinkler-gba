#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include "shrinklergbacore/elfio_wrapper.hpp"
#include "shrinklergbacore/elf_strings.hpp"
#include "shrinklergbacore/input_file.hpp"
#include "shrinklergbacore/table_printer.hpp"

namespace shrinklergbacore
{

using boost::numeric_cast;
using ELFIO::Elf64_Addr;
using ELFIO::Elf_Half;
using ELFIO::segment;
using std::runtime_error;

static table_printer create_table_printer()
{
    table_printer p;
    p.table_indent(2);
    return p;
}

void input_file::load_elf(std::istream& stream)
{
    reset();

    elfio reader;
    open_elf(reader, stream);
    check_header(reader);
    read_entry(reader);
    log_program_headers(reader);
    log_section_headers(reader);
    convert_to_binary(reader);
}

void input_file::log_program_headers(elfio& reader) const
{
    auto printer = create_table_printer();
    printer.add_row({ "Nr", "Type", "Offset", "VirtAddr", "PhysAddr", "FileSiz", "MemSiz", "Align", "Flg" });
    for (Elf_Half i = 0; i < nheaders; ++i)
    {
        const auto& s = *reader.segments[i];
        printer.add_row({
            std::to_string(i),
            elf_strings::get_segment_type(s.get_type()),
            elf_strings::to_hex(s.get_offset(), 6),
            elf_strings::to_hex(s.get_virtual_address(), 8),
            elf_strings::to_hex(s.get_physical_address(), 8),
            elf_strings::to_hex(s.get_file_size(), 5),
            elf_strings::to_hex(s.get_memory_size(), 5),
            elf_strings::to_hex(s.get_align(), 5),
            elf_strings::get_segment_flags(s.get_flags())});
    }

    CONSOLE_VERBOSE(m_console) << "Program headers" << std::endl;
    printer.print(*m_console.verbose());
}

void input_file::log_section_headers(ELFIO::elfio& reader) const
{
    if (!m_console.is_verbose_enabled())
    {
        return;
    }

    Elf_Half nheaders = reader.sections.size();
    if (nheaders == 0)
    {
        CONSOLE_VERBOSE(m_console) << "File has no section headers" << std::endl;
        return;
    }

    auto printer = create_table_printer();
    printer.add_row({ "Nr", "Name", "Type", "Addr", "Off", "Size", "ES", "Flg", "Lk", "Inf", "Al", "Inc" });
    for (Elf_Half i = 0; i < nheaders; ++i)
    {
        const ELFIO::section* s = reader.sections[i];
        printer.add_row({
            std::to_string(i),
            s->get_name(),
            elf_strings::get_section_type(s->get_type()),
            elf_strings::to_hex(s->get_address(), 8),
            elf_strings::to_hex(s->get_offset(), 6),
            elf_strings::to_hex(s->get_size(), 6),
            elf_strings::to_hex(s->get_entry_size(), 2),
            elf_strings::get_section_flags(s->get_flags()),
            elf_strings::to_hex(s->get_link(), 2),
            elf_strings::to_hex(s->get_info(), 3),
            elf_strings::to_hex(s->get_addr_align(), 2),
            is_section_included(s) ? "Y" : "N"
            });
    }

    CONSOLE_VERBOSE(m_console) << "Section headers" << std::endl;
    printer.print(*m_console.verbose());
}

std::vector<const ELFIO::section*> input_file::get_included_sections(ELFIO::elfio& reader)
{
    std::vector<const ELFIO::section*> included_sections;
    std::copy_if(
        reader.sections.begin(),
        reader.sections.end(),
        std::back_inserter(included_sections),
        is_section_included);
    return included_sections;
}

}
