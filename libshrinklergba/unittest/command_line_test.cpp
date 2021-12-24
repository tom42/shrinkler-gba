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

class command_line_test_fixture
{
public:
    libshrinklergba::options options;

    libshrinklergba::command_action parse_command_line(const char* /*command_line*/)
    {
        // TODO: is a fixture instantiated for each test?
        // TODO: argc
        // TODO: argv
        return libshrinklergba::parse_command_line(0, nullptr, options, true);
    }
};

BOOST_FIXTURE_TEST_SUITE(command_line_test, command_line_test_fixture)

    BOOST_AUTO_TEST_CASE(empty_command_line)
    {
        BOOST_TEST((parse_command_line("") == command_action::exit_failure));
    }

    BOOST_AUTO_TEST_CASE(more_than_one_input_file)
    {
        BOOST_TEST((parse_command_line("file1 file2") == command_action::exit_failure));
    }

    BOOST_AUTO_TEST_CASE(one_input_file_no_other_options)
    {
        // TODO: rewrite to use BOOST_TEST
        BOOST_TEST((parse_command_line("file1") == command_action::process));
        BOOST_CHECK_EQUAL("file1", options.input_file());
        BOOST_CHECK_EQUAL("file1.gba", options.output_file());
        BOOST_CHECK_EQUAL(false, options.verbose());
    }

BOOST_AUTO_TEST_SUITE_END()

}
