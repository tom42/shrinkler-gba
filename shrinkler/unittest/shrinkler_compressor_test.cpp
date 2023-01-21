// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <boost/test/unit_test.hpp>
#include "shrinkler/shrinkler.hpp"

namespace shrinkler_unittest
{

using namespace shrinkler;

static std::vector<unsigned char> make_vector(const char* s)
{
    return std::vector<unsigned char>(s, s + strlen(s));
}

BOOST_AUTO_TEST_SUITE(shrinkler_compressor_test)

    BOOST_AUTO_TEST_CASE(compress)
    {
        auto original = make_vector("foo foo foo foo");
        shrinkler_compressor testee;
        testee.set_parameters(shrinkler_parameters(9));

        auto compressed = testee.compress(original);

        unsigned char expected[]{ 0xc6, 0x62, 0xc8, 0x99, 0x00, 0x00, 0x39, 0x9b };
        BOOST_TEST(expected == compressed, boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(compress_no_parity)
    {
        auto original = make_vector("foo foo foo foo");
        shrinkler_parameters parameters(9);
        parameters.parity_context = false;
        shrinkler_compressor testee;
        testee.set_parameters(parameters);

        auto compressed = testee.compress(original);

        unsigned char expected[]{ 0xda, 0x70, 0xc5, 0x99, 0x00, 0x80, 0x59, 0xe3 };
        BOOST_TEST(expected == compressed, boost::test_tools::per_element());
    }

BOOST_AUTO_TEST_SUITE_END()

}
