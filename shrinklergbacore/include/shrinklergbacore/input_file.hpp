// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_INPUT_FILE_HPP
#define SHRINKLERGBACORE_INPUT_FILE_HPP

#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <vector>
#include "shrinklergbacore/console.hpp"

namespace ELFIO
{

class elfio;
class section;

}

namespace shrinklergbacore
{

class input_file final
{
public:
    input_file() : input_file(console()) {}

    explicit input_file(const console& c) : m_console(c) {}

    void load(const std::filesystem::path& path);

    void load(std::istream& stream);

    uint32_t entry() const { return m_entry; }

    bool is_thumb_entry() const { return entry() & 1; }

    uint32_t load_address() const { return m_load_address; }

    const std::vector<unsigned char>& data() const { return m_data; }

private:
    void load_elf(std::istream& stream);
    void reset();
    void read_entry(ELFIO::elfio& reader);
    void log_program_headers(ELFIO::elfio& reader) const;
    void log_section_headers(ELFIO::elfio& reader) const;
    void convert_to_binary(ELFIO::elfio& reader);

    static void open_elf(ELFIO::elfio& reader, std::istream& stream);
    static void check_header(ELFIO::elfio& reader);
    static void check_executable_type(ELFIO::elfio& reader);
    static void check_elf_version(ELFIO::elfio& reader);
    static void check_os_abi(ELFIO::elfio& reader);
    static void check_abi_version(ELFIO::elfio& reader);
    static void check_object_file_version(ELFIO::elfio& reader);

    static bool is_section_included(const ELFIO::section* s);
    static std::vector<const ELFIO::section*> get_included_sections(ELFIO::elfio& reader);
    static void sort_sections_by_address(std::vector<const ELFIO::section*>& sections);

    const console m_console;
    uint32_t m_entry = 0;
    uint32_t m_load_address = 0;
    std::vector<unsigned char> m_data;
};

}

#endif
