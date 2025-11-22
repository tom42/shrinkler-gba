// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::calculate_complement;
using shrinkler_gba::checksum_area_size;
using shrinkler_gba::ofs_game_title;
using shrinkler_gba::ofs_game_version;

namespace
{

const unsigned char all_zero_except_fixed_byte[shrinkler_gba::checksum_area_size] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char ascending_numbers[shrinkler_gba::checksum_area_size] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e
};

}

TEST_CASE("complement")
{
    SECTION("value of checksum_area_size is correct")
    {
        CHECK(checksum_area_size == 30);
    }

    SECTION("calculate complement normally")
    {
        CHECK(calculate_complement(all_zero_except_fixed_byte) == 0x51);
        CHECK(calculate_complement(ascending_numbers) == 0x34);
    }

    SECTION("calculate complement with fixed complement byte")
    {
        std::vector<unsigned char> buf(ascending_numbers, ascending_numbers + std::size(ascending_numbers));

        // We want the real complement byte to have a fixed value, and fix the game version byte instead.
        // Calculate what value the game version byte should have.
        const size_t game_version_index = ofs_game_version - ofs_game_title;
        const unsigned char game_version_byte = calculate_complement(buf.data(), game_version_index);
        CHECK(game_version_byte == 0x33);

        // Patch the game version byte in the checksum area, and check whether everything matches up.
        buf[game_version_index] = game_version_byte;
        const unsigned char calculated_complement_byte = calculate_complement(buf.data());
        CHECK(calculated_complement_byte == 0x1e);
        CHECK(buf[checksum_area_size - 2] == 0x33); // Game version
        CHECK(buf[checksum_area_size - 1] == 0x1e); // Complement byte which we forced to a desired value
    }
}

}
