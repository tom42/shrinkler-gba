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
#include "libshrinkler/libshrinkler.hpp"

namespace libshrinkler_unittest
{

using namespace libshrinkler;

static std::vector<unsigned char> make_vector(const char* s)
{
    return std::vector<unsigned char>(s, s + strlen(s));
}

BOOST_AUTO_TEST_SUITE(shrinkler_test)

    BOOST_AUTO_TEST_CASE(shrinkler_test)
    {
        auto original = make_vector("He who foos last foos best.");
        shrinkler testee;
        testee.set_parameters(shrinkler_parameters(9));

        auto compressed = testee.compress(original);

        // TODO: verify data, somehow
        BOOST_FAIL("YIKES");
    }

BOOST_AUTO_TEST_SUITE_END()

}
