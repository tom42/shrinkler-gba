// MIT License
//
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
// Copyright (c) 2021 Thomas Mathys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
