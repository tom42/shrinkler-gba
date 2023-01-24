// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <boost/algorithm/string.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/test/unit_test.hpp>
#include <cstring>
#include <string>
#include <vector>
#include "shrinklergbacore/command_line.hpp"
#include "shrinklergbacore/options.hpp"

namespace shrinklergbacore_unittest
{

using shrinklergbacore::command_action;
using std::string;
using std::vector;

class command_line_test_fixture
{
public:
    shrinklergbacore::options options;

    shrinklergbacore::command_action parse_command_line(const char* command_line)
    {
        options = shrinklergbacore::options();

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

        return shrinklergbacore::parse_command_line(boost::numeric_cast<int>(argv.size()), argv.data(), options, true);
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
        BOOST_TEST(options.debug_checks() == false);
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

    BOOST_AUTO_TEST_CASE(usage_option)
    {
        BOOST_TEST((parse_command_line("--usage") == command_action::exit_success));
    }

    BOOST_AUTO_TEST_CASE(invalid_option)
    {
        BOOST_TEST((parse_command_line("--invalid-option") == command_action::exit_failure));
    }

    BOOST_AUTO_TEST_CASE(output_file_option_before_input_file)
    {
        BOOST_TEST((parse_command_line("-o output input") == command_action::process));
        BOOST_TEST(options.input_file() == "input");
        BOOST_TEST(options.output_file() == "output");
    }

    BOOST_AUTO_TEST_CASE(output_file_option_after_input_file)
    {
        BOOST_TEST((parse_command_line("input -o output") == command_action::process));
        BOOST_TEST(options.input_file() == "input");
        BOOST_TEST(options.output_file() == "output");
    }

    BOOST_AUTO_TEST_CASE(verbose_option)
    {
        BOOST_TEST((parse_command_line("input -v") == command_action::process));
        BOOST_TEST(options.verbose() == true);
        BOOST_TEST((parse_command_line("input --verbose") == command_action::process));
        BOOST_TEST(options.verbose() == true);
    }

    BOOST_AUTO_TEST_CASE(debug_checks_option)
    {
        BOOST_TEST((parse_command_line("input --debug-checks") == command_action::process));
        BOOST_TEST(options.debug_checks() == true);
    }

    BOOST_AUTO_TEST_CASE(shrinkler_iterations_option)
    {
        BOOST_TEST((parse_command_line("input -i") == command_action::exit_failure));
        BOOST_TEST((parse_command_line("input -i x") == command_action::exit_failure));
        BOOST_TEST((parse_command_line("input -i 0") == command_action::exit_failure));
        BOOST_TEST((parse_command_line("input -i 10") == command_action::exit_failure));

        BOOST_TEST((parse_command_line("input -i 1") == command_action::process));
        BOOST_TEST(options.shrinkler_parameters().iterations == 1);

        BOOST_TEST((parse_command_line("input -i 9") == command_action::process));
        BOOST_TEST(options.shrinkler_parameters().iterations == 9);
    }

    BOOST_AUTO_TEST_CASE(shrinkler_compression_options)
    {
        BOOST_TEST((parse_command_line("input -i1 -l11 -a111 -e1111 -s11111 -r111111") == command_action::process));
        BOOST_TEST(options.shrinkler_parameters().iterations == 1);
        BOOST_TEST(options.shrinkler_parameters().length_margin == 11);
        BOOST_TEST(options.shrinkler_parameters().same_length == 111);
        BOOST_TEST(options.shrinkler_parameters().effort == 1111);
        BOOST_TEST(options.shrinkler_parameters().skip_length == 11111);
        BOOST_TEST(options.shrinkler_parameters().references == 111111);
    }

    BOOST_AUTO_TEST_CASE(shrinkler_preset_option)
    {
        BOOST_TEST((parse_command_line("input -p3") == command_action::process));
        BOOST_TEST(options.shrinkler_parameters().iterations == 3);
        BOOST_TEST(options.shrinkler_parameters().length_margin == 3);
        BOOST_TEST(options.shrinkler_parameters().same_length == 30);
        BOOST_TEST(options.shrinkler_parameters().effort == 300);
        BOOST_TEST(options.shrinkler_parameters().skip_length == 3000);
        BOOST_TEST(options.shrinkler_parameters().references == 100000);
    }

BOOST_AUTO_TEST_SUITE_END()

}
