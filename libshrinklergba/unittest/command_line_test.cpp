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
#include <boost/numeric/conversion/cast.hpp>
#include <boost/test/unit_test.hpp>
#include <cstring>
#include <string>
#include <vector>
#include "libshrinklergba/command_line.hpp"

namespace libshrinklergba_unittest
{

using libshrinklergba::command_action;
using std::string;
using std::vector;

class command_line_test_fixture
{
public:
    libshrinklergba::options options;

    libshrinklergba::command_action parse_command_line(const char* command_line)
    {
        // TODO: is a fixture instantiated for each test?

        // Split string into individual arguments and convert them to vector<char>
        vector<vector<char>> vectors;
        vectors.push_back(to_vector("program_name"));
        if (strlen(command_line))
        {
            vector<string> strings;
            boost::split(strings, command_line, boost::is_any_of(" "));
            for (const auto& s : strings)
            {
                vectors.push_back(to_vector(s));
            }
        }

        // Put together an argv array.
        vector<char*> argv;
        for (auto& v : vectors)
        {
            argv.push_back(v.data());
        }

        return libshrinklergba::parse_command_line(boost::numeric_cast<int>(argv.size()), argv.data(), options, true);
    }

private:
    static vector<char> to_vector(const string& s)
    {
        return vector<char>(s.c_str(), s.c_str() + s.size() + 1);
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
        BOOST_TEST((parse_command_line("file1") == command_action::process));
        BOOST_TEST(options.input_file() == "file1");
        BOOST_TEST(options.output_file() == "file1.gba");
        BOOST_TEST(options.verbose() == false);
    }

    BOOST_AUTO_TEST_CASE(help_option)
    {
        BOOST_TEST((parse_command_line("-?") == command_action::exit_success));
        BOOST_TEST((parse_command_line("--help") == command_action::exit_success));
    }

    BOOST_AUTO_TEST_CASE(version_option)
    {
        BOOST_TEST((parse_command_line("-V") == command_action::exit_success));
        BOOST_TEST((parse_command_line("--version") == command_action::exit_success));
    }

BOOST_AUTO_TEST_SUITE_END()

}
