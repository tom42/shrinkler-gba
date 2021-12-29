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

#include <fstream>
#include <stdexcept>
#include <system_error>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4458)
#endif

#include "elfio/elfio.hpp"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "shrinklergba/input_file.hpp"

namespace shrinklergba
{

using ELFIO::elfio;
using std::runtime_error;

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
    throw "YIKES";

    // TODO: port stuff below
    /*
    read_entry(reader);
    log_program_headers(reader);
    convert_to_binary(reader);

    // TODO: that should not go into load_elf, but into load, no?
    CONSOLE_VERBOSE(m_console) << format("Entry: {:#x}", m_entry) << std::endl;
    CONSOLE_VERBOSE(m_console) << format("Load address: {:#x}", m_load_address) << std::endl;
    CONSOLE_VERBOSE(m_console) << format("Total size of loaded data: {0:#x} ({0})", m_data.size()) << std::endl;
    */
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

    // TODO: port stuff below
    /*
    check_elf_version(reader);

    // Not sure these matter. Checking them to be on the safe side.
    check_os_abi(reader);
    check_abi_version(reader);
    check_object_file_version(reader);

    // TODO: store this, we're going to need it.
    //       Do we limit the entry points we are going to accept? Well perhaps, but not in this class.
    */
}

void input_file::check_executable_type(ELFIO::elfio& /*reader*/)
{
    // TODO: implement (need a happy path test first)
}

}
