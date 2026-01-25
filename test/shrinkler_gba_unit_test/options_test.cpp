// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <utility>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using options = shrinkler_gba::options;
using std::make_pair;

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

    SECTION("input file sets output file if not yet set")
    {
        auto [input_file, expected_output_file] = GENERATE(
            make_pair("input.elf", "input.gba"),
            make_pair("directory/input.elf", "input.gba"));

        options testee;
        testee.input_file(input_file);
        CHECK(testee.output_file() == expected_output_file);
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
