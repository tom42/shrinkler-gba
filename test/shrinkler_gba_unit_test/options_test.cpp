// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using options = shrinkler_gba::options;

TEST_CASE("options")
{
    SECTION("constructor")
    {
        const options testee;
        CHECK(testee.input_file() == "");
        CHECK(testee.output_file() == "");
        CHECK(testee.verbose() == false);
        CHECK(testee.code_in_header() == true);
        CHECK(testee.debug_checks() == false);
        CHECK(testee.packer().has_value() == false);
    }
}

}
