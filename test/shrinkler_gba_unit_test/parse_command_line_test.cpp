// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

import argpppp;
import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::parse_command_line_result;
using std::make_pair;
using std::string;
using std::vector;

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

        CHECK(result.success == false);
    }

    SECTION("more than one input file")
    {
        auto result = parse_command_line("file1 file2");

        CHECK(result.success == false);
    }

    SECTION("one input file, no other options")
    {
        auto result = parse_command_line("file1.elf");

        CHECK(result.success == true);
        CHECK(result.options.input_file() == "file1.elf");
        CHECK(result.options.output_file() == "file1.gba");
        CHECK(result.options.verbose() == false);
        CHECK(result.options.code_in_header() == true);
        CHECK(result.options.debug_checks() == false);
    }

    SECTION("invalid option")
    {
        auto result = parse_command_line("--invalid-option");

        CHECK(result.success == false);
    }

    SECTION("output file option before input file argument")
    {
        auto result = parse_command_line("-o output input");

        CHECK(result.success == true);
        CHECK(result.options.input_file() == "input");
        CHECK(result.options.output_file() == "output");
    }

    SECTION("output file option after input file argument")
    {
        auto result = parse_command_line("input -o output");

        CHECK(result.success == true);
        CHECK(result.options.input_file() == "input");
        CHECK(result.options.output_file() == "output");
    }

    SECTION("--verbose option")
    {
        auto command_line = GENERATE(
            "input -v",
            "input --verbose");

        auto result = parse_command_line(command_line);

        CHECK(result.success == true);
        CHECK(result.options.verbose() == true);
    }

    SECTION("--no-code-in-header option")
    {
        auto result = parse_command_line("input --no-code-in-header");

        CHECK(result.success == true);
        CHECK(result.options.code_in_header() == false);
    }

    SECTION("--debug-checks option")
    {
        auto result = parse_command_line("input --debug-checks");

        CHECK(result.success == true);
        CHECK(result.options.debug_checks() == true);
    }

    SECTION("--preset option")
    {
        auto result = parse_command_line("input -p2");

        CHECK(result.success == true);
        CHECK(result.options.shrinkler_parameters().iterations() == 2);
        CHECK(result.options.shrinkler_parameters().length_margin() == 2);
        CHECK(result.options.shrinkler_parameters().same_length() == 20);
        CHECK(result.options.shrinkler_parameters().effort() == 200);
        CHECK(result.options.shrinkler_parameters().skip_length() == 2000);
        CHECK(result.options.shrinkler_parameters().references() == 100000);
    }

    SECTION("--iterations option")
    {
        auto [command_line, expected_iterations] = GENERATE(
            make_pair("input -i1", 1),
            make_pair("input -i9", 9));

        auto result = parse_command_line(command_line);

        CHECK(result.success == true);
        CHECK(result.options.shrinkler_parameters().iterations() == expected_iterations);
    }
}

}

// TODO: get test up and running
/*
    BOOST_AUTO_TEST_CASE(shrinkler_iterations_option)
    {
        BOOST_TEST((parse_command_line("input -i") == command_action::exit_failure));
        BOOST_TEST((parse_command_line("input -i x") == command_action::exit_failure));
        BOOST_TEST((parse_command_line("input -i 0") == command_action::exit_failure));
        BOOST_TEST((parse_command_line("input -i 10") == command_action::exit_failure));
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
}
*/
