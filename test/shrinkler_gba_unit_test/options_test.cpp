// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using options = shrinkler_gba::options;

TEST_CASE("options_test")
{

    SECTION("constructor")
    {
        const options testee;
        CHECK(testee.input_file() == "");
        CHECK(testee.output_file() == "");
        CHECK(testee.verbose() == false);
        CHECK(testee.code_in_header() == true);
        CHECK(testee.debug_checks() == false);
    }

    SECTION("input file sets output file if not yet set")
    {
        options testee;
        testee.input_file("input.elf");
        CHECK(testee.output_file() == "input.gba");
    }

    SECTION("input file does not set output file if already set")
    {
        options testee;

        testee.output_file("output.gba");
        testee.input_file("input.bin");

        CHECK(testee.input_file() == "input.bin");
        CHECK(testee.output_file() == "output.gba");
    }

}

}
