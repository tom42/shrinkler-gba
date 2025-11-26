// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <array>
#include "shrinklergbacore/elf_strings.hpp"

namespace shrinklergbacore::elf_strings
{

std::string get_segment_type(ELFIO::Elf_Word type)
{
    static const std::array table{ "NULL", "LOAD", "DYNAMIC", "INTERP", "NOTE", "SHLIB", "PHDR", "TLS" };

    if (type < table.size())
    {
        return table[type];
    }

    return to_hex(type, 8);
}

std::string get_segment_flags(ELFIO::Elf_Word flags)
{
    static const std::array table{ "", "X", "W", "WX", "R", "RX", "RW", "RWX" };

    if (flags < table.size())
    {
        return table[flags];
    }

    return to_hex(flags);
}

}
