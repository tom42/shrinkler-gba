// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <string>
#include "version.hpp"

module shrinkler_gba;
import argpppp;

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
using std::string;

namespace
{

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
        ;

    argpppp::command_line_parser parser;
    parser.flags(flags_for_unit_test);
    parser.parse(argc, argv, options);

    // TODO: fill in result
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

    // TODO: argpppp improvements:
    //       * It is not immediately obvious what the default value for parser flags would be. Either rename 'none' to 'default',
    //         or, alternatively, have an additional enum member defined as such: 'default = none'

    add_option(parser, { "debug-checks", option::debug_checks, {}, {}, "Add debug checks to depacker code" }, [&](auto) { result.options.debug_checks(true); return true; });

    add_header(parser, "Shrinkler compression options (default values in parentheses):");
    add_option(parser, { "same-length", 'a', "N", {}, "Number of matches of the same length to consider (20)" }, bogus_callback);
    add_option(parser, { "effort", 'e', "N", {}, "Perseverance in finding multiple matches (200)" }, bogus_callback);
    add_option(parser, { "iterations", 'i', "N", {}, "Number of iterations for the compression (2)" }, bogus_callback);
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
