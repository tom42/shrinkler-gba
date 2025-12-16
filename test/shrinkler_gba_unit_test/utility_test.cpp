// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <cstdint>
#include <limits>
#include <type_traits>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::to_signed;

TEMPLATE_TEST_CASE("to_signed", "", uint16_t, uint32_t)
{
    constexpr auto unsigned_value = std::numeric_limits<TestType>::max();

    constexpr auto signed_value = to_signed(unsigned_value);

    CHECK(sizeof(signed_value) == sizeof(unsigned_value));
    CHECK(std::is_signed_v<decltype(signed_value)> == true);
    CHECK(signed_value == -1);
}

}
