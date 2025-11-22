// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <vector>
#include "test_utilities.hpp"

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::adler32;

TEST_CASE("adler32")
{
    CHECK(adler32(make_bytevector("")) == 0x00000001u);
    CHECK(adler32(make_bytevector("a")) == 0x00620062u);
    CHECK(adler32(make_bytevector("abc")) == 0x024d0127u);
    CHECK(adler32(load_binary_file("lostmarbles.bin")) == 0xeadcf4a9);
}

}
