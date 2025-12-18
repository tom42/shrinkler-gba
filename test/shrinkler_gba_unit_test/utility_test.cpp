// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <cstdint>
#include <limits>
#include <type_traits>
#include "test_utilities.hpp"

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::adler32;
using shrinkler_gba::to_signed;

TEST_CASE("adler32")
{
    CHECK(adler32(make_bytevector("")) == 0x00000001u);
    CHECK(adler32(make_bytevector("a")) == 0x00620062u);
    CHECK(adler32(make_bytevector("abc")) == 0x024d0127u);
    CHECK(adler32(load_binary_file("lostmarbles.bin")) == 0xeadcf4a9);
}

TEMPLATE_TEST_CASE("to_signed", "", uint16_t, uint32_t)
{
    constexpr auto unsigned_value = std::numeric_limits<TestType>::max();

    constexpr auto signed_value = to_signed(unsigned_value);

    CHECK(sizeof(signed_value) == sizeof(unsigned_value));
    CHECK(std::is_signed_v<decltype(signed_value)> == true);
    CHECK(signed_value == -1);
}

}
