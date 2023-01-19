// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <array>
#include "shrinklergba/elf_strings.hpp"

namespace shrinklergba::elf_strings
{

std::string get_section_type(ELFIO::Elf_Word type)
{
    switch (type)
    {
        case SHT_NULL: return "NULL";
        case SHT_PROGBITS: return "PROGBITS";
        case SHT_SYMTAB: return "SYMTAB";
        case SHT_STRTAB: return "STRTAB";
        case SHT_RELA: return "RELA";
        case SHT_HASH: return "HASH";
        case SHT_DYNAMIC: return "DYNAMIC";
        case SHT_NOTE: return "NOTE";
        case SHT_NOBITS: return "NOBITS";
        case SHT_REL: return "REL";
        case SHT_SHLIB: return "SHLIB";
        case SHT_DYNSYM: return "DYNSYM";
        case SHT_INIT_ARRAY: return "INIT_ARRAY";
        case SHT_FINI_ARRAY: return "FINI_ARRAY";
        case SHT_PREINIT_ARRAY: return "PREINIT_ARRAY";
        case SHT_GROUP: return "GROUP";
        case SHT_SYMTAB_SHNDX: return "SYMTAB_SHNDX";
        case 0x70000003: return "ARM_ATTRIBUTES";
        default:
            return to_hex(type, 8);
    }
}

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
