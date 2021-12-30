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

// Some quotes from the ELF specifications:
//
// Segment type PT_LOAD:
//
// "The array element specifies a loadable segment, described by p_filesz and p_memsz.
// The bytes from the file are mapped to the beginning of the memory segment.
// If the segment's memory size (p_memsz) is larger than the file size (p_filesz),
// the ``extra'' bytes are defined to hold the value 0 and to follow the segment's
// initialized area. The file size may not be larger than the memory size.
// Loadable segment entries in the program header table appear in ascending order,
// sorted on the p_vaddr member."
//
// p_align field of segment header:
//
// "Loadable process segments must have congruent values for p_vaddr and p_offset,
// modulo the page size. This member gives the value to which the segments are aligned
// in memory and in the file.
// Values 0 and 1 mean no alignment is required. Otherwise, p_align should be a positive,
// integral power of 2, and p_vaddr should equal p_offset, modulo p_align."

#include <array>
#include <fstream>
#include <stdexcept>
#include <string>
#include <system_error>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4458)
#endif

#include "elfio/elfio.hpp"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "fmt/core.h"
#include "shrinklergba/input_file.hpp"

namespace shrinklergba
{

using ELFIO::elfio;
using ELFIO::Elf_Half;
using ELFIO::Elf_Word;
using fmt::format;
using std::runtime_error;
using std::string;

static string segment_type_to_string(Elf_Word type)
{
    static const std::array table{ "NULL", "LOAD", "DYNAMIC", "INTERP", "NOTE", "SHLIB", "PHDR", "TLS" };

    if ((type >= 0) && (type < table.size()))
    {
        return table[type];
    }

    return format("{:#010x}", type);
}

static string segment_flags_to_string(Elf_Word /*flags*/)
{
    // TODO: implement
    return "***";
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
}

void input_file::load_elf(std::istream& stream)
{
    // TODO: reset fields here

    elfio reader;

    open_elf(reader, stream);
    check_header(reader);
    read_entry(reader);
    log_program_headers(reader);
    throw "YIKES";

    // TODO: port stuff below
    /*
    convert_to_binary(reader);

    // TODO: that should not go into load_elf, but into load, no? (since it is not ELF specific)
    CONSOLE_VERBOSE(m_console) << format("Entry: {:#x}", m_entry) << std::endl;
    CONSOLE_VERBOSE(m_console) << format("Load address: {:#x}", m_load_address) << std::endl;
    CONSOLE_VERBOSE(m_console) << format("Total size of loaded data: {0:#x} ({0})", m_data.size()) << std::endl;
    */
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

    CONSOLE_VERBOSE(m_console) << "Program headers" << std::endl;
    CONSOLE_VERBOSE(m_console) << format(" {:10} {:7} {:10} {:10} {:7} {:7} {:7} {:3}",
        "Type",
        "Offset",
        "VirtAddr",
        "PhysAddr",
        "FileSiz",
        "MemSiz",
        "Align",
        "Flg") << std::endl;

    for (Elf_Half i = 0; i < nheaders; ++i)
    {
        const auto& s = *reader.segments[i];
        CONSOLE_VERBOSE(m_console) << format(" {:10} {:#07x} {:#010x} {:#010x} {:#07x} {:#07x} {:#07x} {}",
            segment_type_to_string(s.get_type()),
            s.get_offset(),
            s.get_virtual_address(),
            s.get_physical_address(),
            s.get_file_size(),
            s.get_memory_size(),
            s.get_align(),
            segment_flags_to_string(s.get_flags())) << std::endl;
    }
}

void input_file::open_elf(elfio& reader, std::istream& stream)
{
    if (!reader.load(stream))
    {
        throw runtime_error("file is not a valid ELF file");
    }
}

void input_file::check_header(ELFIO::elfio& reader)
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

void input_file::check_executable_type(ELFIO::elfio& reader)
{
    if ((reader.get_class() != ELFCLASS32) ||
        (reader.get_encoding() != ELFDATA2LSB) ||
        (reader.get_type() != ET_EXEC) ||
        (reader.get_machine() != EM_ARM))
    {
        throw runtime_error("file is not a 32-bit little endian ARM executable ELF file");
    }
}

void input_file::check_elf_version(ELFIO::elfio& reader)
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

}
