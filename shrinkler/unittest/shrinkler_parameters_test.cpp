// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <boost/test/unit_test.hpp>
#include "shrinkler/shrinkler.hpp"

namespace shrinkler_unittest
{

using namespace shrinkler;

BOOST_AUTO_TEST_SUITE(shrinkler_parameters_test)

    BOOST_AUTO_TEST_CASE(constructor_default_preset)
    {
        const shrinkler_parameters testee;

        BOOST_TEST(testee.parity_context == true);
        BOOST_TEST(testee.iterations == 2);
        BOOST_TEST(testee.length_margin == 2);
        BOOST_TEST(testee.same_length == 20);
        BOOST_TEST(testee.effort == 200);
        BOOST_TEST(testee.skip_length == 2000);
        BOOST_TEST(testee.references == 100000);
        BOOST_TEST(testee.verbose == false);
    }

    BOOST_AUTO_TEST_CASE(constructor_preset_9)
    {
        const shrinkler_parameters testee(9);

        BOOST_TEST(testee.parity_context == true);
        BOOST_TEST(testee.iterations == 9);
        BOOST_TEST(testee.length_margin == 9);
        BOOST_TEST(testee.same_length == 90);
        BOOST_TEST(testee.effort == 900);
        BOOST_TEST(testee.skip_length == 9000);
        BOOST_TEST(testee.references == 100000);
        BOOST_TEST(testee.verbose == false);
    }

BOOST_AUTO_TEST_SUITE_END()

}
