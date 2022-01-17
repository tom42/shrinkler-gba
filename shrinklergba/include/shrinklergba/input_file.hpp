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

#ifndef SHRINKLERGBA_INPUT_FILE_HPP
#define SHRINKLERGBA_INPUT_FILE_HPP

#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <vector>
#include "shrinklergba/console.hpp"

namespace ELFIO
{

class elfio;
class section;
class segment;

}

namespace shrinklergba
{

class input_file
{
public:
    input_file() : input_file(console()) {}

    explicit input_file(const console& c) : m_console(c) {}

    void load(const std::filesystem::path& path);

    void load(std::istream& stream);

    uint_fast64_t entry() const { return m_entry; }

    bool is_thumb_entry() const { return entry() & 1; }

    uint_fast64_t load_address() const { return m_load_address; }

    const std::vector<unsigned char>& data() const { return m_data; }

private:
    void load_elf(std::istream& stream);
    void read_entry(ELFIO::elfio& reader);
    void log_program_headers(ELFIO::elfio& reader) const;
    void log_section_headers(ELFIO::elfio& reader) const;
    void convert_to_binary(ELFIO::elfio& reader);
    void convert_to_binary_old(ELFIO::elfio& reader);

    static void open_elf(ELFIO::elfio& reader, std::istream& stream);
    static void check_header(ELFIO::elfio& reader);
    static void check_executable_type(ELFIO::elfio& reader);
    static void check_elf_version(ELFIO::elfio& reader);
    static void check_os_abi(ELFIO::elfio& reader);
    static void check_abi_version(ELFIO::elfio& reader);
    static void check_object_file_version(ELFIO::elfio& reader);

    static bool is_section_included(const ELFIO::section* s);
    static std::vector<const ELFIO::section*> get_included_sections(ELFIO::elfio& reader);
    static void verify_load_segment(ELFIO::segment* last, ELFIO::segment* current);
    static void throw_if_invalid_load_segment(ELFIO::segment* seg);
    static void throw_if_load_segments_are_out_of_order(ELFIO::segment* last, ELFIO::segment* current);
    static void throw_if_load_segments_overlap(ELFIO::segment* last, ELFIO::segment* current);

    const console m_console;
    uint_fast64_t m_entry = 0;
    uint_fast64_t m_load_address = 0;
    std::vector<unsigned char> m_data;
};

}

#endif
