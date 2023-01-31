// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <boost/test/unit_test.hpp>
#include "shrinklergbacore/adler32.hpp"
#include "test_utilities.hpp"

namespace shrinklergbacore_unittest
{

using shrinklergbacore::adler32;

BOOST_AUTO_TEST_SUITE(adler32_test)

    BOOST_AUTO_TEST_CASE(test)
    {
        BOOST_TEST(adler32(make_bytevector("")) == 0x00000001u);
        BOOST_TEST(adler32(make_bytevector("a")) == 0x00620062u);
        BOOST_TEST(adler32(make_bytevector("abc")) == 0x024d0127u);
        BOOST_TEST(adler32(load_binary_file("lostmarbles.bin")) == 0xeadcf4a9);
    }

BOOST_AUTO_TEST_SUITE_END()

}
