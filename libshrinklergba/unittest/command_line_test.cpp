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
#include "libshrinklergba/command_line.hpp"

namespace libshrinklergba_unittest
{

using libshrinklergba::command_action;
using libshrinklergba::parse_command_line;

class command_line_test_fixture
{
public:
    libshrinklergba::command_action parse_command_line(const char* /*command_line*/)
    {
        // TODO: actually run command line parser
        return command_action::exit_failure;
    }
};

BOOST_FIXTURE_TEST_SUITE(command_line_test, command_line_test_fixture)

    BOOST_AUTO_TEST_CASE(empty_command_line)
    {
        // TODO: get rid of casts
        BOOST_TEST((int)parse_command_line("") == (int)command_action::exit_failure);
    }

BOOST_AUTO_TEST_SUITE_END()

}
