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
