// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <format>
#include <string>
#include "version.hpp"

module shrinkler_gba;
import argpppp;
import libshrinkler;

// extern "C" is needed for some platforms, e.g. when using MSVC and argp-standalone.
// It is not needed for glibc.
extern "C"
{
// TODO: does this work on GNU/Linux?
const char* argp_program_version = SHRINKLER_GBA_PROJECT_NAME " " SHRINKLER_GBA_PROJECT_VERSION;
}

namespace shrinkler_gba
{

using argpppp::set;
using std::format;
using std::string;
using namespace libshrinkler;

namespace
{

// TODO: extend argpppp, so that non-printable option keys can be assigned automatically. We should not have to define these keys, they have no meaning for us.
//       * Maybe we can extend argpppp::option: it should not take ints as keys, but some sort of type, e.g. option_key, or short_name
//       * Probably we want an implicit conversion from char to short_name. That will be how options with printable short name will primarily be defined
//       * We can then have all sorts of special cased things, for cases like
//         * "I want to assign a code manually" (then again, maybe we should not support this in the first place - yagni)
//         * "This is a special option"
//         * "This option has no short name"
//       When done this enum can go.
enum option
{
    first = 256,
    no_code_in_header,
    debug_checks,
};

}

parse_command_line_result parse_command_line(int argc, char* argv[], argpppp::pf flags_for_unit_test)
{
    parse_command_line_result result;
    argpppp::options options;
    options
        .doc(
            SHRINKLER_GBA_PROJECT_NAME " - Shrinkler for the Gameboy Advance by Tom/Vantage\n"
            "Shrinkler compression by Blueberry/Loonies\n"
            "https://github.com/tom42/shrinkler-gba")
        .args_doc("FILE")
        .num_args(1)

        .add_header("General options:")
        .add({ 'o', "output-file", "Output file name. Default is input file name with extension replaced by .gba", "FILE" },
            set<string>([&](string s) { result.options.output_file(s); }))
        .add({ 'v', "verbose", "Print verbose messages" }, set<bool>([&](bool) { result.options.verbose(true); }))

        .add_header("Depacker options:")
        .add({ option::no_code_in_header, "no-code-in-header", "Do not put code into ROM header" },
            set<bool>([&](bool) { result.options.code_in_header(false); } ))
        .add({ option::debug_checks, "debug-checks", "Add debug checks to depacker code" },
            set<bool>([&](bool) { result.options.debug_checks(true); }))

        .add_header("Shrinkler compression options (default values in parentheses):")
        .add({ 'a', "same-length", format("Number of matches of same length to consider ({})", result.options.shrinkler_parameters().same_length()), "N" },
            set<int>([&](int n) { result.options.shrinkler_parameters().same_length(n); })
            .min(min_same_length)
            .max(max_same_length))
        .add({ 'e', "effort", format("Perseverance in finding multiple matches ({})", result.options.shrinkler_parameters().effort()), "N" },
            set<int>([&](int n) { result.options.shrinkler_parameters().effort(n); })
            .min(min_effort)
            .max(max_effort))
        .add({ 'i', "iterations", format("Number of compression iterations ({})", result.options.shrinkler_parameters().iterations()), "N" },
            set<int>([&](int n) { result.options.shrinkler_parameters().iterations(n); })
            .min(min_iterations)
            .max(max_iterations))
        .add({ 'l', "length-margin", format("Number of shorter matches considered for each match ({})", result.options.shrinkler_parameters().length_margin()), "N" },
            set<int>([&](int n) { result.options.shrinkler_parameters().length_margin(n); })
            .min(min_length_margin)
            .max(max_length_margin))
        .add({ 'p', "preset", format("Preset for all compression options except --references ({}..{}, default {})", min_preset, max_preset, default_preset), "PRESET"},
            set<int>([&](int n) { result.options.shrinkler_parameters().preset(n); })
            .min(min_preset)
            .max(max_preset))
        .add({ 'r', "references", format("Number of reference edges to keep in memory ({})", result.options.shrinkler_parameters().references()), "N" },
            set<int>([&](int n) { result.options.shrinkler_parameters().references(n); })
            .min(min_references)
            .max(max_references))
        .add({ 's', "skip-length", format("Minimum match length to accept greedily ({})", result.options.shrinkler_parameters().skip_length()), "N" },
            set<int>([&](int n) { result.options.shrinkler_parameters().skip_length(n); })
            .min(min_skip_length)
            .max(max_skip_length))
        ;

    argpppp::command_line_parser parser;
    parser.flags(flags_for_unit_test);
    auto parse_result = parser.parse(argc, argv, options);

    result.success = parse_result.errnum == 0;
    result.options.input_file(parse_result.args.at(0));

    return result;
}

}
