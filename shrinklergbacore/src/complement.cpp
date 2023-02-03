// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include "shrinklergbacore/complement.hpp"
#include "shrinklergbacore/gba.hpp"

namespace shrinklergbacore
{

unsigned char calculate_complement(const std::vector<unsigned char>& header_data)
{
    unsigned char sum = 0x19;
    for (size_t n = ofs_game_title; n < ofs_complement; ++n)
    {
        sum += header_data[n];
    }

    unsigned char complement = -sum;
    return complement;
}

}
