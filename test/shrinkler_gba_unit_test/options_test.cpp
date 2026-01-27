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
        CHECK(testee.input_file == "");
        CHECK(testee.output_file == "");
        CHECK(testee.verbose == false);
        CHECK(testee.code_in_header == true);
        CHECK(testee.debug_checks == false);
        CHECK(testee.packer.has_value() == false);
    }

    SECTION("has_output_file")
    {
        const auto [output_file, expected_result] = GENERATE(
            make_pair("", false),
            make_pair("bar.elf", true),
            make_pair("foo/bar.elf", true));

        options testee;
        testee.output_file = output_file;

        CHECK(has_output_file(testee) == expected_result);
    }

    SECTION("has_output_file, no output file set")
    {
        const options testee;
        CHECK(has_output_file(testee) == false);
    }
}

}
