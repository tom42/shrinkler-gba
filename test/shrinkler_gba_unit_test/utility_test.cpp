// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <type_traits>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::to_signed;

TEST_CASE("utility_test")
{
    SECTION("to_signed")
    {
        constexpr auto unsigned_value = uint16_t(0xffff);

        constexpr auto signed_value = to_signed(unsigned_value);

        CHECK(sizeof(signed_value) == sizeof(unsigned_value));
        CHECK(std::is_signed_v<decltype(signed_value)> == true);
        CHECK(signed_value == -1);
        // TODO: check another type?
    }
}

}
