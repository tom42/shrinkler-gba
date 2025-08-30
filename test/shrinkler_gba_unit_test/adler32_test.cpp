// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <vector>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

namespace
{

// TODO: might want to move this later into some common place
const std::vector<unsigned char> make_bytevector(const char* s)
{
    return std::vector<unsigned char>(s, s + std::strlen(s));
}

}

using shrinkler_gba::adler32;

TEST_CASE("adler32_test")
{
    CHECK(adler32(make_bytevector("")) == 0x00000001u);
    CHECK(adler32(make_bytevector("a")) == 0x00620062u);
    CHECK(adler32(make_bytevector("abc")) == 0x024d0127u);
}

}

// TODO: turn into a catch2 test: need to get adler32 wprking before we worry about load_binary_file
//    BOOST_AUTO_TEST_CASE(test)
//    {
//        BOOST_TEST(adler32(load_binary_file("lostmarbles.bin")) == 0xeadcf4a9);
//    }
