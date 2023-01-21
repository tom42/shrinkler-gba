// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_ELF_STRINGS_HPP
#define SHRINKLERGBACORE_ELF_STRINGS_HPP

#include <string>
#include "fmt/core.h"
#include "shrinklergbacore/elfio_wrapper.hpp"

namespace shrinklergbacore::elf_strings
{

template <typename TValue>
std::string to_hex(TValue value)
{
    return fmt::format("{:#0x}", value);
}

template <typename TValue, typename NDigits>
std::string to_hex(TValue value, NDigits ndigits)
{
    const auto prefix_width = 2;
    return fmt::format("{:#0{}x}", value, ndigits + prefix_width);
}

std::string get_section_type(ELFIO::Elf_Word type);
std::string get_section_flags(ELFIO::Elf_Xword flags);
std::string get_segment_type(ELFIO::Elf_Word type);
std::string get_segment_flags(ELFIO::Elf_Word flags);

}

#endif
