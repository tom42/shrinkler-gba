// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <array>
#include "shrinklergbacore/elf_strings.hpp"

namespace shrinklergbacore::elf_strings
{

std::string get_section_flags(ELFIO::Elf_Xword flags)
{
    struct table_entry
    {
        ELFIO::Elf_Xword flag;
        char character;
    };

    static const table_entry table[] =
    {
        {SHF_WRITE, 'W'},
        {SHF_ALLOC, 'A'},
        {SHF_EXECINSTR, 'X'},
        {SHF_MERGE, 'M'},
        {SHF_STRINGS, 'S'}
    };

    std::string result;

    for (const auto& entry : table)
    {
        if (flags & entry.flag)
        {
            result += entry.character;
            flags &= ~entry.flag;
        }
    }

    if (flags)
    {
        return to_hex(flags);
    }

    return result;
}

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
