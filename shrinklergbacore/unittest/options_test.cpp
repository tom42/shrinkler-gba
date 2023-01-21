// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <boost/test/unit_test.hpp>
#include "shrinklergbacore/options.hpp"

namespace shrinklergbacore_unittest
{

using shrinklergbacore::options;

BOOST_AUTO_TEST_SUITE(options_test)

    BOOST_AUTO_TEST_CASE(constructor)
    {
        const options testee;
        BOOST_TEST(testee.input_file() == "");
        BOOST_TEST(testee.output_file() == "");
        BOOST_TEST(testee.verbose() == false);
    }

    BOOST_AUTO_TEST_CASE(input_file_sets_output_file_if_not_yet_set)
    {
        options testee;
        testee.input_file("input.elf");
        BOOST_TEST(testee.output_file() == "input.gba");
    }

    BOOST_AUTO_TEST_CASE(input_file_does_not_set_output_file_if_already_set)
    {
        options testee;

        testee.output_file("output.gba");
        testee.input_file("input.bin");

        BOOST_TEST(testee.input_file() == "input.bin");
        BOOST_TEST(testee.output_file() == "output.gba");
    }

BOOST_AUTO_TEST_SUITE_END()

}
