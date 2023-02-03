// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include "shrinklergbacore/complement.hpp"
#include "shrinklergbacore/gba.hpp"

namespace shrinklergbacore
{

unsigned char calculate_complement(const unsigned char* game_title)
{
    constexpr size_t complement_index = ofs_complement - ofs_game_title;
    unsigned char sum = 0x19;

    for (size_t i = 0; i < complement_area_size; ++i)
    {
        if (i != complement_index)
        {
            sum += game_title[i];
        }
    }

    unsigned char complement = -sum;
    return complement;
}

}
