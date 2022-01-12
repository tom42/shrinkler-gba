// MIT License
//
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
// Copyright (c) 2021 Thomas Mathys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <boost/numeric/conversion/cast.hpp>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include "fmt/core.h"
#include "shrinklergba/elfio_wrapper.hpp"
#include "shrinklergba/elf_strings.hpp"
#include "shrinklergba/input_file.hpp"
#include "shrinklergba/table_printer.hpp"

namespace shrinklergba
{

using ELFIO::elfio;
using ELFIO::Elf64_Addr;
using ELFIO::Elf_Half;
using ELFIO::segment;
using fmt::format;
using std::runtime_error;

static table_printer create_table_printer()
{
    table_printer p;
    p.table_indent(2);
    return p;
}

void input_file::load(const std::filesystem::path& path)
{
    try
    {
        CONSOLE_VERBOSE(m_console) << "Loading: " << path.string() << std::endl;
        std::ifstream stream(path, std::ios::binary);

        if (!stream)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        load(stream);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(path.string() + ": " + e.what());
    }
}

void input_file::load(std::istream& stream)
{
    load_elf(stream);

    CONSOLE_VERBOSE(m_console) << format("Entry: {:#x}", m_entry) << std::endl;
    CONSOLE_VERBOSE(m_console) << format("Load address: {:#x}", m_load_address) << std::endl;
    CONSOLE_VERBOSE(m_console) << format("Total size of loaded data: {0:#x} ({0})", m_data.size()) << std::endl;
}

void input_file::load_elf(std::istream& stream)
{
    // TODO: reset fields here

    elfio reader;

    open_elf(reader, stream);
    check_header(reader);
    read_entry(reader);
    log_program_headers(reader);
    log_section_headers(reader);
    convert_to_binary(reader);
}

void input_file::read_entry(elfio& reader)
{
    m_entry = reader.get_entry();
}

void input_file::log_program_headers(elfio& reader)
{
    if (!m_console.is_verbose_enabled())
    {
        return;
    }

    Elf_Half nheaders = reader.segments.size();
    if (nheaders == 0)
    {
        CONSOLE_VERBOSE(m_console) << "File has no program headers" << std::endl;
        return;
    }

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

void input_file::log_section_headers(ELFIO::elfio& reader)
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
        const auto& s = *reader.sections[i];
        printer.add_row({
            std::to_string(i),
            s.get_name(),
            elf_strings::get_section_type(s.get_type()),
            elf_strings::to_hex(s.get_address(), 8),
            elf_strings::to_hex(s.get_offset(), 6),
            elf_strings::to_hex(s.get_size(), 6),
            elf_strings::to_hex(s.get_entry_size(), 2),
            elf_strings::get_section_flags(s.get_flags()),
            elf_strings::to_hex(s.get_link(), 2),
            elf_strings::to_hex(s.get_info(), 3),
            elf_strings::to_hex(s.get_addr_align(), 2),
            include_section(s) ? "Y" : "N"
            });
    }

    CONSOLE_VERBOSE(m_console) << "Section headers" << std::endl;
    printer.print(*m_console.verbose());
}

bool input_file::include_section(const ELFIO::section& s)
{
    // TODO: Info whether we keep or discard a section (keep/discard means it goes into bin file or not) => Should be "Y/N" for the moment
    //       Go through elf docs:
    //       * NOBITS? What else?
    //       * ...
    /*
    // TODO: go through each of these fields and see whether it can/should/must be used for filtering out of unneeded sections
typedef struct {
        Elf32_Word      sh_flags;       =>  Is there something important with the alloc bit? Can we throw out sections that do not have it set?
        Elf32_Off       sh_offset;      =>  Might have to special handle this along with NOBITS
        Elf32_Word      sh_size;
} Elf32_Shdr;
    */

    if (s.get_type() == SHT_NULL)
    {
        // * SHT_NULL should be ignored: https://stackoverflow.com/questions/26812142/what-is-the-use-of-the-sht-null-section-in-elf
        //   Quote from the ELF specifications:
        //     "This value marks the section header as inactive; it does not have an associated section.
        //     Other members of the section header have undefined values."
        //  The latter bit means that we must not look at any other fields of a section header of type SHT_NULL.

        // TODO: are there any other section types that do not need processing at all? => Go through list of section types defined by ELFIO and ARM elf specs
        // TODO: possibly need to filter out NOBITS sections, since, according to ELF specs, NOBITS section to not occupy space inside ELF files
        return false;
    }

    // TODO: check sh_flags before sh_addr?

    if (s.get_address() == 0)
    {
        // ELF specifications:
        //   "If the section will appear in the memory image of a process, this member
        //   gives the address at which the section's first byte should reside. Otherwise,
        //   the member contains 0."
        return false;
    }

    return true;
}

void input_file::convert_to_binary(elfio& reader)
{
    // TODO: Do we initially check whether there are any program headers?
    //       Or do we simply do all the processing and fail if there is no data left?
    segment* last = nullptr;
    const Elf_Half nheaders = reader.segments.size();
    Elf64_Addr output_address = 0;

    for (Elf_Half i = 0; i < nheaders; ++i)
    {
        segment* current = reader.segments[i];
        if (current->get_type() == PT_LOAD)
        {
            verify_load_segment(last, current);

            if (current->get_file_size())
            {
                if (m_data.size())
                {
                    // Move to start of segment in output. Fill up with padding bytes.
                    const auto nbytes = current->get_virtual_address() - output_address;
                    m_data.insert(m_data.end(), boost::numeric_cast<size_t>(nbytes), 0);
                    output_address += nbytes;
                }
                else
                {
                    // No bytes written to output yet. Just set the output address then.
                    output_address = current->get_virtual_address();
                    m_load_address = output_address;
                }

                // Copy segment's data to output.
                m_data.insert(m_data.end(), current->get_data(), current->get_data() + current->get_file_size());
                output_address += current->get_file_size();
            }

            // TODO: make padding byte value configurable? Compressed size? Otoh, 0xff might be more healthy for flash devices?
            //       * Careful: ELF might require that it is 0x00.
            //       * The argument regarding flash devices is moot: we're going to compress that data afterwards, so it won't
            //         be 0xff anymore.
            // TODO: final size checks(?)
            //       * Should we check whether there are any bytes at all?
            //       * Should we check for a maximum size?

            last = current;
        }
    }
}

void input_file::open_elf(elfio& reader, std::istream& stream)
{
    if (!reader.load(stream))
    {
        throw runtime_error("file is not a valid ELF file");
    }
}

void input_file::check_header(elfio& reader)
{
    check_executable_type(reader);
    check_elf_version(reader);

    // Not sure these matter. Checking them to be on the safe side.
    check_os_abi(reader);
    check_abi_version(reader);
    check_object_file_version(reader);

    // TODO: store this, we're going to need it.
    //       * Do we limit the entry points we are going to accept? Well perhaps, but not in this class.
    //       * Well basically entries must be in IWRAM or EWRAM. Or, more specifically, inside the
    //         memory area occupied by the binary. But then, that's probably too much to worry about
}

void input_file::check_executable_type(elfio& reader)
{
    if ((reader.get_class() != ELFCLASS32) ||
        (reader.get_encoding() != ELFDATA2LSB) ||
        (reader.get_type() != ET_EXEC) ||
        (reader.get_machine() != EM_ARM))
    {
        throw runtime_error("file is not a 32-bit little endian ARM executable ELF file");
    }
}

void input_file::check_elf_version(elfio& reader)
{
    const auto expected_elf_version = 1;

    auto ei_version = reader.get_elf_version();
    if (ei_version != expected_elf_version)
    {
        throw runtime_error(format("unknown ELF format version {}. Expected {}", ei_version, expected_elf_version));
    }
}

void input_file::check_os_abi(elfio& reader)
{
    const auto expected_abi = ELFOSABI_NONE;

    auto ei_osabi = reader.get_os_abi();
    if (ei_osabi != expected_abi)
    {
        throw runtime_error(format("unknown ELF OS ABI {}. Expected none ({})", ei_osabi, expected_abi));
    }
}

void input_file::check_abi_version(elfio& reader)
{
    const auto expected_abi_version = 0;

    auto ei_abiversion = reader.get_abi_version();
    if (ei_abiversion != expected_abi_version)
    {
        throw runtime_error(format("unknown ABI version {}. Expected {}", ei_abiversion, expected_abi_version));
    }
}

void input_file::check_object_file_version(elfio& reader)
{
    const auto expected_object_file_version = 1;

    auto e_version = reader.get_version();
    if (e_version != expected_object_file_version)
    {
        throw runtime_error(format("unknown object file version {}. Expected {}", e_version, expected_object_file_version));
    }
}

void input_file::verify_load_segment(segment* last, segment* current)
{
    throw_if_invalid_load_segment(current);

    if (last)
    {
        throw_if_load_segments_are_out_of_order(last, current);
        throw_if_load_segments_overlap(last, current);
    }
}

void input_file::throw_if_invalid_load_segment(segment* seg)
{
    // Not sure this is a problem. Refuse to process such a file until we know.
    if (seg->get_virtual_address() != seg->get_physical_address())
    {
        throw runtime_error("file contains LOAD segment whose virtual and physical address differ. Don't know how to handle that");
    }

    // Required by ELF specifications.
    if (seg->get_file_size() > seg->get_memory_size())
    {
        throw runtime_error("invalid ELF file. Found LOAD segment whose file size is larger than its memory size");
    }

    // Not sure this could be a problem, but for the time being require
    // segment alignment to be according to ELF specifications.
    const auto align = seg->get_align();
    if (align > 1)
    {
        if ((align & (align - 1)) != 0)
        {
            throw runtime_error("invalid ELF file. Found LOAD segment whose alignment is not a power of 2");
        }

        if ((seg->get_offset() % align) != (seg->get_virtual_address() % align))
        {
            throw runtime_error("invalid ELF file. Found LOAD segment where (offset % align) != (virtual address % align)");
        }
    }
}

void input_file::throw_if_load_segments_are_out_of_order(segment* last, segment* current)
{
    // Required by ELF specifications.
    if (current->get_virtual_address() < last->get_virtual_address())
    {
        throw runtime_error("invalid ELF file. LOAD segment headers are not sorted by ascending virtual address");
    }
}

void input_file::throw_if_load_segments_overlap(segment* last, segment* current)
{
    if ((last->get_virtual_address() + last->get_memory_size()) < last->get_virtual_address())
    {
        throw runtime_error("invalid ELF file. Found LOAD segment that goes past the end of the 64-bit address space");
    }

    if ((last->get_virtual_address() + last->get_memory_size()) > current->get_virtual_address())
    {
        throw runtime_error("invalid ELF file. Found overlapping LOAD segments");
    }
}

}
