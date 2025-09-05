// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>
#include <vector>

import argpppp;
import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using std::string;
using std::vector;
using shrinkler_gba::parse_command_line_result;

namespace
{

vector<char> to_vector(const string& s)
{
    return vector<char>(s.c_str(), s.c_str() + s.size() + 1);
}

parse_command_line_result parse_command_line(const char* command_line)
{
    using argpppp::pf;

    // Split string into individual arguments
    vector<vector<char>> args;
    args.push_back(to_vector("program_name"));
    std::istringstream stream(command_line);
    string s;
    while (stream >> s)
    {
        args.push_back(to_vector(s));
    }

    // Put together argv array
    vector<char*> argv;
    for (auto& arg : args)
    {
        argv.push_back(arg.data());
    }

    return shrinkler_gba::parse_command_line(static_cast<int>(argv.size()), argv.data(), pf::no_errs | pf::no_exit);
}

}

TEST_CASE("parse_command_line_test")
{
    SECTION("empty command line")
    {
        auto result = parse_command_line("");
        CHECK(result.parse_result == false);
    }

    SECTION("more than one input file")
    {
        auto result = parse_command_line("file1 file2");
        CHECK(result.parse_result == false);
    }

    SECTION("one input file, no other options")
    {
        auto result = parse_command_line("file1.elf");
        CHECK(result.options.input_file() == "file1.elf");
        CHECK(result.options.output_file() == "file1.gba");
        CHECK(result.options.verbose() == false);
        CHECK(result.options.code_in_header() == true);
        CHECK(result.options.debug_checks() == false);
    }
}

}

// TODO: get test up and running
/*
BOOST_FIXTURE_TEST_SUITE(command_line_test, command_line_test_fixture)


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

    BOOST_AUTO_TEST_CASE(no_code_in_header_option)
    {
        BOOST_TEST((parse_command_line("input --no-code-in-header") == command_action::process));
        BOOST_TEST(options.code_in_header() == false);
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
*/