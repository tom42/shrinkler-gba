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

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include "shrinklergba/input_file.hpp"

namespace shrinklergba_unittest
{

using std::runtime_error;
using shrinklergba::input_file;

BOOST_AUTO_TEST_SUITE(input_file_test)

    BOOST_AUTO_TEST_CASE(constructor)
    {
        const input_file testee;

        BOOST_TEST(testee.entry() == 0);
        BOOST_TEST(testee.load_address() == 0);
        BOOST_TEST(testee.data().size() == 0);
    }

    BOOST_AUTO_TEST_CASE(load_when_file_does_not_exist_then_throws)
    {
        input_file testee;

        BOOST_CHECK_EXCEPTION(
            testee.load("non-existing-file.elf"),
            runtime_error,
            [](const auto& e) { return boost::iequals("non-existing-file.elf: no such file or directory", e.what()); });
    }

BOOST_AUTO_TEST_SUITE_END()

}