// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <concepts>
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

}
