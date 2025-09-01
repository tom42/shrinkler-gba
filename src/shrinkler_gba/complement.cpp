// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstddef>

module shrinkler_gba;

namespace shrinkler_gba
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
