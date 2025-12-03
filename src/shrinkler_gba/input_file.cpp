// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <elfio/elfio.hpp>
#include <gsl/gsl>
#include <ranges>
#include <stdexcept>
#include <string>

module shrinkler_gba;
import :elf_strings;

namespace shrinkler_gba
{

using ELFIO::elfio;

namespace
{

void check_executable_type(const elfio& elfio)
{
    if ((elfio.get_class() != ELFIO::ELFCLASS32) ||
        (elfio.get_encoding() != ELFIO::ELFDATA2LSB) ||
        (elfio.get_type() != ELFIO::ET_EXEC) ||
        (elfio.get_machine() != ELFIO::EM_ARM))
    {
        throw std::runtime_error("File is not a 32-bit little endian ARM executable ELF file");
    }
}

void check_elf_version(const elfio& elfio)
{
    const auto expected_elf_version = 1;

    auto version = elfio.get_elf_version();
    if (version != expected_elf_version)
    {
        throw std::runtime_error(std::format("Unknown ELF format version {}. Expected {}", version, expected_elf_version));
    }
}

void check_os_abi(const elfio& elfio)
{
    const auto expected_abi = ELFIO::ELFOSABI_NONE;

    auto osabi = elfio.get_os_abi();
    if (osabi != expected_abi)
    {
        throw std::runtime_error(std::format("Unknown ELF OS ABI {}. Expected none ({})", osabi, expected_abi));
    }
}

void check_abi_version(const elfio& elfio)
{
    const auto expected_abi_version = 0;

    auto abiversion = elfio.get_abi_version();
    if (abiversion != expected_abi_version)
    {
        throw std::runtime_error(std::format("Unknown ABI version {}. Expected {}", abiversion, expected_abi_version));
    }
}

void check_object_file_version(const elfio& elfio)
{
    const auto expected_object_file_version = 1;

    auto e_version = elfio.get_version();
    if (e_version != expected_object_file_version)
    {
        throw std::runtime_error(std::format("Unknown object file version {}. Expected {}", e_version, expected_object_file_version));
    }
}

void check_header(const elfio& elfio)
{
    check_executable_type(elfio);
    check_elf_version(elfio);
    check_os_abi(elfio);
    check_abi_version(elfio);
    check_object_file_version(elfio);
}

uint32_t read_entry(const elfio& elfio)
{
    return gsl::narrow<uint32_t>(elfio.get_entry());
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

std::vector<const ELFIO::section*> get_included_sections(const elfio& elfio)
{
    return
        std::views::transform(elfio.sections, [](const auto& s) { return s.get(); }) |
        std::views::filter(is_section_included) |
        std::ranges::to<std::vector<const ELFIO::section*>>();
}

void sort_sections_by_address(std::vector<const ELFIO::section*>& sections)
{
    std::sort(
        sections.begin(),
        sections.end(),
        [](const ELFIO::section* lhs, const ELFIO::section* rhs) { return lhs->get_address() < rhs->get_address(); });
}

std::vector<unsigned char> convert_to_binary(const elfio& elfio, uint32_t& out_load_address)
{
    out_load_address = 0;

    std::vector<const ELFIO::section*> included_sections = get_included_sections(elfio);
    sort_sections_by_address(included_sections);

    const ELFIO::section* previous_section = nullptr;
    ELFIO::Elf64_Addr output_address = 0;
    std::vector<unsigned char> data;

    for (const ELFIO::section* s : included_sections)
    {
        if (data.size())
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
                data.insert(data.end(), gsl::narrow<size_t>(npadding_bytes), 0);
                output_address += npadding_bytes;
            }
        }
        else
        {
            // No bytes written to output yet. Record initial output address and load address.
            output_address = s->get_address();
            out_load_address = gsl::narrow<uint32_t>(output_address);
        }

        // Copy section data to output.
        data.insert(data.end(), s->get_data(), s->get_data() + s->get_size());
        output_address += s->get_size();
        previous_section = s;
    }

    return data;
}

}

input_file::input_file(const elfio& elfio)
{
    // TODO: implement
    check_header(elfio);
    m_entry = read_entry(elfio); // TOOD: make this a member
    m_data = convert_to_binary(elfio, m_load_address); // TODO: make this a member. Benefit: no output parameter anymore
}

}
