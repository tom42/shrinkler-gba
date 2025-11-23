// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <elfio/elfio.hpp>
#include <fstream>
#include <stdexcept>
#include <string>
#include <system_error>

module shrinkler_gba;

namespace shrinkler_gba
{

using ELFIO::elfio;

namespace
{

void open_elf(elfio& reader, std::istream& stream)
{
    if (!reader.load(stream))
    {
        throw std::runtime_error("File is not a valid ELF file");
    }
}

void check_executable_type(elfio& reader)
{
    if ((reader.get_class() != ELFIO::ELFCLASS32) ||
        (reader.get_encoding() != ELFIO::ELFDATA2LSB) ||
        (reader.get_type() != ELFIO::ET_EXEC) ||
        (reader.get_machine() != ELFIO::EM_ARM))
    {
        throw std::runtime_error("File is not a 32-bit little endian ARM executable ELF file");
    }
}

void check_elf_version(elfio& reader)
{
    const auto expected_elf_version = 1;

    auto ei_version = reader.get_elf_version();
    if (ei_version != expected_elf_version)
    {
        throw std::runtime_error(std::format("Unknown ELF format version {}. Expected {}", ei_version, expected_elf_version));
    }
}

void check_os_abi(elfio& reader)
{
    const auto expected_abi = ELFIO::ELFOSABI_NONE;

    auto ei_osabi = reader.get_os_abi();
    if (ei_osabi != expected_abi)
    {
        throw std::runtime_error(std::format("Unknown ELF OS ABI {}. Expected none ({})", ei_osabi, expected_abi));
    }
}

void check_abi_version(elfio& reader)
{
    const auto expected_abi_version = 0;

    auto ei_abiversion = reader.get_abi_version();
    if (ei_abiversion != expected_abi_version)
    {
        throw std::runtime_error(std::format("Unknown ABI version {}. Expected {}", ei_abiversion, expected_abi_version));
    }
}

void check_object_file_version(elfio& reader)
{
    const auto expected_object_file_version = 1;

    auto e_version = reader.get_version();
    if (e_version != expected_object_file_version)
    {
        throw std::runtime_error(std::format("Unknown object file version {}. Expected {}", e_version, expected_object_file_version));
    }
}

void check_header(elfio& reader)
{
    check_executable_type(reader);
    check_elf_version(reader);

    // Not sure these matter. Checking them to be on the safe side.
    check_os_abi(reader);
    check_abi_version(reader);
    check_object_file_version(reader);
}

uint32_t read_entry(elfio& reader)
{
    // TODO: use some sort of narrow_cast that does throw if the value is too big
    return static_cast<uint32_t>(reader.get_entry());
}

}

void input_file::load(const std::string& path, const console& console)
{
    try
    {
        console.verbose("Loading: {}", path);
        std::ifstream stream(path, std::ios::binary);

        if (!stream)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        load(stream, console);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(path + ": " + e.what());
    }
}

void input_file::load(std::istream& stream, const console& console)
{
    load_elf(stream);
    console.verbose("Entry: {:#x}", entry());
    console.verbose("Load address: {:#x}", load_address());
    console.verbose("Total size of loaded data: {0:#x} ({0})", loaded_data_size());
}

void input_file::load_elf(std::istream& stream)
{
    reset();

    elfio reader;
    open_elf(reader, stream);
    check_header(reader);
    m_entry = read_entry(reader);

    // TODO: implement stuff below
    /*
    log_program_headers(reader);
    log_section_headers(reader);
    convert_to_binary(reader);
    */
}

void input_file::reset()
{
    m_entry = 0;
    m_load_address = 0;
    std::vector<unsigned char>().swap(m_data);
}

}
