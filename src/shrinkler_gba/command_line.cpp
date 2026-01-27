// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include "version.hpp"

module shrinkler_gba;
import argpppp;
import libshrinkler;

// extern "C" is needed for some platforms, e.g. when using MSVC and argp-standalone.
// It is not needed for glibc.
extern "C"
{
const char* argp_program_version = SHRINKLER_GBA_PROJECT_NAME " " SHRINKLER_GBA_PROJECT_VERSION;
}

namespace shrinkler_gba
{

using argpppp::callback;
using argpppp::value;
using argpppp::set;
using std::format;
using std::string;
using namespace std::string_literals;
using namespace libshrinkler;
namespace fs = std::filesystem;

namespace
{

std::string packer_list()
{
    std::string list("\n  best");

    for (const auto& packer_info : packer_registry::all_info())
    {
        list += "\n  ";
        list += packer_info.name;
    }

    return list;
}

fs::path default_output_file(const fs::path& input_file)
{
    return input_file.filename().replace_extension("gba");
}

}

parse_command_line_result parse_command_line(int argc, char* argv[], argpppp::pf flags_for_unit_test)
{
    parse_command_line_result result;
    bool no_code_in_header = !result.opts.code_in_header;
    argpppp::options options;
    options
        .doc(
            SHRINKLER_GBA_PROJECT_NAME " - Shrinkler for the Gameboy Advance by Tom/Vantage\n"
            "Shrinkler compression by Blueberry/Loonies\n"
            "https://github.com/tom42/shrinkler-gba")
        .args_doc("FILE")
        .num_args(1)

        .add_header("General options:")
        .add({ 'o', "output-file", "Output file name. Default is input file name with extension replaced by .gba", "FILE" }, callback(
            [&](const auto&, const char* arg)
            {
                // TODO: this is a workaround: we would really like argpppp::value to work with std::filesystem::path
                //       * We would really like it wo work with anything that is assignable from const char*, no?
                result.opts.output_file = arg;
                return argpppp::ok();
            }))
        .add({ 'v', "verbose", "Print verbose messages" }, value(result.opts.verbose))
        .add({ {}, "packer", "Select packer. Case sensitive, default is 'best':" + packer_list(), "PACKER"}, callback( // TODO: have a dedicated option_handler for this, eg parse_packer()
            [&](const auto& opt, const char* arg)
            {
                if (arg == "best"s)
                {
                    result.opts.packer.reset();
                    return argpppp::ok();
                }

                if (!packer_registry::find_info(arg))
                {
                    return argpppp::error(opt, arg, "unknown packer");
                }

                result.opts.packer = arg;
                return argpppp::ok();
            }))

        .add_header("Depacker options:")
        .add({ {}, "no-code-in-header", "Do not put code into ROM header"}, value(no_code_in_header))
        .add({ {}, "debug-checks", "Add debug checks to depacker code"}, value(result.opts.debug_checks))

        .add_header("Shrinkler compression options (default values in parentheses):")
        .add({ 'a', "same-length", format("Number of matches of same length to consider ({})", result.opts.shrinkler_parameters.same_length()), "N" },
            set<int>([&](int n) { result.opts.shrinkler_parameters.same_length(n); })
            .min(min_same_length)
            .max(max_same_length))
        .add({ 'e', "effort", format("Perseverance in finding multiple matches ({})", result.opts.shrinkler_parameters.effort()), "N" },
            set<int>([&](int n) { result.opts.shrinkler_parameters.effort(n); })
            .min(min_effort)
            .max(max_effort))
        .add({ 'i', "iterations", format("Number of compression iterations ({})", result.opts.shrinkler_parameters.iterations()), "N" },
            set<int>([&](int n) { result.opts.shrinkler_parameters.iterations(n); })
            .min(min_iterations)
            .max(max_iterations))
        .add({ 'l', "length-margin", format("Number of shorter matches considered for each match ({})", result.opts.shrinkler_parameters.length_margin()), "N" },
            set<int>([&](int n) { result.opts.shrinkler_parameters.length_margin(n); })
            .min(min_length_margin)
            .max(max_length_margin))
        .add({ 'p', "preset", format("Preset for all compression options except --references ({}..{}, default {})", min_preset, max_preset, default_preset), "PRESET"},
            set<int>([&](int n) { result.opts.shrinkler_parameters.preset(n); })
            .min(min_preset)
            .max(max_preset))
        .add({ 'r', "references", format("Number of reference edges to keep in memory ({})", result.opts.shrinkler_parameters.references()), "N" },
            set<int>([&](int n) { result.opts.shrinkler_parameters.references(n); })
            .min(min_references)
            .max(max_references))
        .add({ 's', "skip-length", format("Minimum match length to accept greedily ({})", result.opts.shrinkler_parameters.skip_length()), "N" },
            set<int>([&](int n) { result.opts.shrinkler_parameters.skip_length(n); })
            .min(min_skip_length)
            .max(max_skip_length))
        ;

    argpppp::command_line_parser parser;
    parser.flags(flags_for_unit_test);
    auto parse_result = parser.parse(argc, argv, options);

    result.success = parse_result.errnum == 0;
    if (result.success)
    {
        result.opts.code_in_header = !no_code_in_header; // TODO: workaround: negative logic needed because argpppp does not support negative logic on value<bool>()
        result.opts.input_file = parse_result.args.at(0);
        if (!has_output_file(result.opts))
        {
            result.opts.output_file = default_output_file(result.opts.input_file);
        }
    }

    return result;
}

}
