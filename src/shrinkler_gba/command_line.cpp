// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <format>
#include <iostream> // TODO: delete once done
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

// TODO: use all arguments
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
                .min(libshrinkler::min_same_length)
                .max(libshrinkler::max_same_length))
        .add({ 'e', "effort", format("Perseverance in finding multiple matches ({})", result.options.shrinkler_parameters().effort()), "N" },
            set<int>([&](int n) { result.options.shrinkler_parameters().effort(n); })
                .min(libshrinkler::min_effort)
                .max(libshrinkler::max_effort))
        .add({ 'i', "iterations", format("Number of compression iterations ({})", result.options.shrinkler_parameters().iterations()), "N" },
            set<int>([&](int n) { result.options.shrinkler_parameters().iterations(n); })
            .min(libshrinkler::min_iterations)
            .max(libshrinkler::max_iterations))
        ;

    argpppp::command_line_parser parser;
    parser.flags(flags_for_unit_test);
    auto parse_result = parser.parse(argc, argv, options);

    // TODO: fill in result (err...what?)
    result.options.input_file(parse_result.args.at(0));

    // TODO: debug code: print options
    std::cout << "input_file:     " << result.options.input_file() << "\n";
    std::cout << "output_file:    " << result.options.output_file() << "\n";
    std::cout << "verbose:        " << result.options.verbose() << "\n";
    std::cout << "code_in_header: " << result.options.code_in_header() << "\n";
    std::cout << "debug_checks:   " << result.options.debug_checks() << "\n";
    std::cout << "same_length:    " << result.options.shrinkler_parameters().same_length() << "\n";
    std::cout << "effort:         " << result.options.shrinkler_parameters().effort() << "\n";
    std::cout << "iterations:     " << result.options.shrinkler_parameters().iterations() << "\n";

    return result;
}

// TODO: redo stuff below
/*
parse_command_line_result parse_command_line(int argc, char* argv[], argpppp::pf flags_for_unit_test)
{
    // TODO: parse command line using argpppp
    //       * Should now add callbacks and set values
    // TODO: are the defaults still good? Can/should we calculate them somehow? (no they're not good anymore, since the default preset is now 3)

    // TODO: set up callbacks to fill in command line options
    parse_command_line_result result;
    argpppp::command_line_parser parser;

    add_option(parser, { "length-margin", 'l', "N", {}, "Number of shorter matches considered for each match (2)" }, bogus_callback);
    add_option(parser, { "preset", 'p', "PRESET", {}, "Preset for all compression options except --references (1..9, default 2)" }, bogus_callback);
    add_option(parser, { "references", 'r', "N", {}, "Number of reference edges to keep in memory (100000)" }, bogus_callback);
    add_option(parser, { "skip-length", 's', "N", {}, "Minimum match length to accept greedily (2000)" }, bogus_callback);

    auto parse_result = parser.parse(argc, argv, flags_for_unit_test);

    // We let argp_parse exit if it wants to, so no need to worry about the --help, --usage or --version option.
    // If parsing is successful and there's a file to compress we return true, otherwise we return false.
    // The parser is set up so that it fails if there is not exactly one argument (the input file).
    result.success = parse_result.errnum == 0;

    if (parse_result.args.size() > 0)
    {
        result.options.input_file(parse_result.args[0]);
    }

    return result;
}*/

}
