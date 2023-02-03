// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <boost/test/unit_test.hpp>
#include "shrinklergbacore/complement.hpp"

namespace shrinklergbacore_unittest
{

using namespace shrinklergbacore;

static const unsigned char all_zero_except_fixed_byte[checksum_area_size] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char ascending_numbers[checksum_area_size] =
{
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e
};

BOOST_AUTO_TEST_SUITE(complement_test)

    BOOST_AUTO_TEST_CASE(checksum_area_size_is_correct)
    {
        BOOST_TEST(checksum_area_size == 30);
    }

    BOOST_AUTO_TEST_CASE(calculate_complement_normally)
    {
        BOOST_TEST(calculate_complement(all_zero_except_fixed_byte) == 0x51);
        BOOST_TEST(calculate_complement(ascending_numbers) == 0x34);
    }

    BOOST_AUTO_TEST_CASE(calculate_complement_with_fixed_complement_byte)
    {
        std::vector<unsigned char> buf(ascending_numbers, ascending_numbers + std::size(ascending_numbers));

        // We want the real complement byte to have a fixed value, and fix the game version byte instead.
        // Calculate what value the game version byte should have.
        const size_t game_version_index = ofs_game_version - ofs_game_title;
        const unsigned char game_version_byte = calculate_complement(buf.data(), game_version_index);
        BOOST_TEST(game_version_byte == 0x33);

        // Patch the game version byte in the checksum area, and check whether everything matches up.
        buf[game_version_index] = game_version_byte;
        const unsigned char calculated_complement_byte = calculate_complement(buf.data());
        BOOST_TEST(calculated_complement_byte == 0x1e);
        BOOST_TEST(buf[checksum_area_size - 2] == 0x33); // Game version
        BOOST_TEST(buf[checksum_area_size - 1] == 0x1e); // Complement byte which we forced to a desired value
    }

BOOST_AUTO_TEST_SUITE_END()

}
