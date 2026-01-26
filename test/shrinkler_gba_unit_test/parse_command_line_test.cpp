// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <optional>
#include <sstream>
#include <string_view>
#include <string>
#include <utility>
#include <vector>

import argpppp;
import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::parse_command_line_result;
using std::optional;
using std::make_pair;
using std::string;
using std::string_view;
using std::vector;
using namespace std::string_literals;

namespace
{

vector<char> to_zstring_vector(string_view s)
{
    vector<char> v;
    v.reserve(s.size() + 1);
#ifdef __cpp_lib_containers_ranges
    v.append_range(s);
#else
    v.insert(v.end(), s.begin(), s.end());
#endif
    v.push_back('\0');
    return v;
}

parse_command_line_result parse_command_line(const char* command_line)
{
    using argpppp::pf;

    // Split string into individual arguments
    vector<vector<char>> args;
    args.push_back(to_zstring_vector("program_name"));
    std::istringstream stream(command_line);
    string s;
    while (stream >> s)
    {
        args.push_back(to_zstring_vector(s));
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

TEST_CASE("parse_command_line")
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
        CHECK(result.opts.input_file() == "file1.elf");
        CHECK(result.opts.output_file() == "file1.gba");
        CHECK(result.opts.verbose() == false);
        CHECK(result.opts.code_in_header() == true);
        CHECK(result.opts.debug_checks() == false);
        CHECK(result.opts.packer().has_value() == false);
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
        CHECK(result.opts.input_file() == "input");
        CHECK(result.opts.output_file() == "output");
    }

    SECTION("output file option after input file argument")
    {
        auto result = parse_command_line("input -o output");

        CHECK(result.success == true);
        CHECK(result.opts.input_file() == "input");
        CHECK(result.opts.output_file() == "output");
    }

    SECTION("--verbose option")
    {
        auto command_line = GENERATE(
            "input -v",
            "input --verbose");

        auto result = parse_command_line(command_line);

        CHECK(result.success == true);
        CHECK(result.opts.verbose() == true);
    }

    SECTION("--no-code-in-header option")
    {
        auto result = parse_command_line("input --no-code-in-header");

        CHECK(result.success == true);
        CHECK(result.opts.code_in_header() == false);
    }

    SECTION("--debug-checks option")
    {
        auto result = parse_command_line("input --debug-checks");

        CHECK(result.success == true);
        CHECK(result.opts.debug_checks() == true);
    }

    SECTION("--packer option")
    {
        const auto [command_line, expected_packer] = GENERATE(
            make_pair("input --packer=best", optional<string>{}),
            make_pair("input --packer=lzss", optional("lzss"s)),
            make_pair("input --packer=shrinkler", optional("shrinkler"s)));

        auto result = parse_command_line(command_line);

        CHECK(result.success == true);
        CHECK(result.opts.packer() == expected_packer);
    }

    SECTION("--packer option, 'best' clears previously set value")
    {
        auto result = parse_command_line("input --packer=shrinkler --packer=best");

        CHECK(result.success == true);
        CHECK(result.opts.packer() == optional<string>{});
    }

    SECTION("--packer option, missing or bad argument")
    {
        auto command_line = GENERATE(
            "input --packer",
            "input --packer=bad_packer");

        auto result = parse_command_line(command_line);

        CHECK(result.success == false);
    }

    SECTION("--preset option")
    {
        auto result = parse_command_line("input -p2");

        CHECK(result.success == true);
        CHECK(result.opts.shrinkler_parameters().iterations() == 2);
        CHECK(result.opts.shrinkler_parameters().length_margin() == 2);
        CHECK(result.opts.shrinkler_parameters().same_length() == 20);
        CHECK(result.opts.shrinkler_parameters().effort() == 200);
        CHECK(result.opts.shrinkler_parameters().skip_length() == 2000);
        CHECK(result.opts.shrinkler_parameters().references() == 100000);
    }

    SECTION("--iterations option")
    {
        const auto [command_line, expected_iterations] = GENERATE(
            make_pair("input -i1", 1),
            make_pair("input -i9", 9));

        auto result = parse_command_line(command_line);

        CHECK(result.success == true);
        CHECK(result.opts.shrinkler_parameters().iterations() == expected_iterations);
    }

    SECTION("--iterations option, missing or bad argument")
    {
        auto command_line = GENERATE(
            "input -i",
            "input -i x",
            "input -i 0",
            "input -i 10");

        auto result = parse_command_line(command_line);

        CHECK(result.success == false);
    }

    SECTION("shrinkler compression options")
    {
        auto result = parse_command_line("input -i1 -l11 -a111 -e1111 -s11111 -r111111");

        CHECK(result.success == true);
        CHECK(result.opts.shrinkler_parameters().iterations() == 1);
        CHECK(result.opts.shrinkler_parameters().length_margin() == 11);
        CHECK(result.opts.shrinkler_parameters().same_length() == 111);
        CHECK(result.opts.shrinkler_parameters().effort() == 1111);
        CHECK(result.opts.shrinkler_parameters().skip_length() == 11111);
        CHECK(result.opts.shrinkler_parameters().references() == 111111);
    }
}

}
