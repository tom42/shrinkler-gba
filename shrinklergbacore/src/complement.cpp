// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include "shrinklergbacore/complement.hpp"
#include "shrinklergbacore/gba.hpp"

namespace shrinklergbacore
{

unsigned char calculate_complement(const unsigned char* checksum_area)
{
    constexpr size_t complement_index = ofs_complement - ofs_game_title;
    return calculate_complement(checksum_area, complement_index);
}

unsigned char calculate_complement(const unsigned char* checksum_area, size_t variable_byte_index)
{
    unsigned char sum = 0x19;

    for (size_t i = 0; i < checksum_area_size; ++i)
    {
        if (i != variable_byte_index)
        {
            sum += checksum_area[i];
        }
    }

    unsigned char complement = -sum;
    return complement;
}

}
