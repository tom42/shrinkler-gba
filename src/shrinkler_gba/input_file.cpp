// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <elfio/elfio.hpp>
#include <fstream>
#include <gsl/gsl>
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
    return gsl::narrow<uint32_t>(reader.get_entry());
}

bool is_section_included(const ELFIO::section* s)
{
    if ((s->get_type() == ELFIO::SHT_NULL) || (s->get_type() == ELFIO::SHT_NOBITS))
    {
        return false;
    }

    if (s->get_address() == 0)
    {
        return false;
    }

    if (s->get_size() == 0)
    {
        return false;
    }

    if (!(s->get_flags() & ELFIO::SHF_ALLOC))
    {
        return false;
    }

    return true;
}

// TODO: can we implement this again using copy_if as before?
//       Problem is that iterators are now suddenly unique_ptrs. Sigh.
std::vector<const ELFIO::section*> get_included_sections(ELFIO::elfio& reader)
{
    const ELFIO::Elf_Half nsections = reader.sections.size();
    std::vector<const ELFIO::section*> sections;

    for (ELFIO::Elf_Half i = 0; i < nsections; ++i)
    {
        if (is_section_included(reader.sections[i]))
        {
            sections.push_back(reader.sections[i]);
        }
    }

    return sections;
}

void sort_sections_by_address(std::vector<const ELFIO::section*>& sections)
{
    std::sort(
        sections.begin(),
        sections.end(),
        [](const ELFIO::section* lhs, const ELFIO::section* rhs) { return lhs->get_address() < rhs->get_address(); });
}

std::vector<unsigned char> convert_to_binary(ELFIO::elfio& reader, uint32_t& load_address)
{
    std::vector<const ELFIO::section*> included_sections = get_included_sections(reader);
    sort_sections_by_address(included_sections);

    const ELFIO::section* previous_section = nullptr;
    ELFIO::Elf64_Addr output_address = 0;
    std::vector<unsigned char> m_data; // TODO: rename to 'data'

    for (const ELFIO::section* s : included_sections)
    {
        if (m_data.size())
        {
            if (s->get_address() < output_address)
            {
                throw std::runtime_error(std::format("Section {} overlaps with previous section {}", s->get_name(), previous_section->get_name()));
            }

            const auto npadding_bytes = s->get_address() - output_address;
            if (npadding_bytes > 0)
            {
                // There is a hole between the current and the last section.
                // Pad it with zeros. Zeros are required by ELF.
                m_data.insert(m_data.end(), static_cast<size_t>(npadding_bytes), 0); // TODO: numeric cast
                output_address += npadding_bytes;
            }
        }
        else
        {
            // No bytes written to output yet. Record initial output address and load address.
            output_address = s->get_address();
            load_address = static_cast<uint32_t>(output_address); // TODO: numeric cast
        }

        // Copy section data to output.
        m_data.insert(m_data.end(), s->get_data(), s->get_data() + s->get_size());
        output_address += s->get_size();
        previous_section = s;
    }

    return m_data;
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
    //log_program_headers(reader); // TODO: implement (skipped to get tests going first)
    //log_section_headers(reader); // TODO: implement (skipped to get tests going first)
    m_data = convert_to_binary(reader, m_load_address);
}

void input_file::reset()
{
    m_entry = 0;
    m_load_address = 0;
    std::vector<unsigned char>().swap(m_data);
}

}
