// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <concepts>
#include <elfio/elfio.hpp>
#include <format>
#include <string>

export module shrinkler_gba:elf_strings;

namespace shrinkler_gba
{

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
template <std::unsigned_integral TValue>
std::string to_hex(TValue value)
{
    return std::format("{:#0x}", value);
}

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
template <std::unsigned_integral TValue, std::integral NDigits>
std::string to_hex(TValue value, NDigits ndigits)
{
    const auto prefix_width = 2;
    return std::format("{:#0{}x}", value, ndigits + prefix_width);
}

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
std::string get_section_type(ELFIO::Elf_Word type);

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
std::string get_section_flags(ELFIO::Elf_Xword flags);

std::string get_segment_type(ELFIO::Elf_Word type);

std::string get_segment_flags(ELFIO::Elf_Word flags);

}
