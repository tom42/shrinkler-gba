// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

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

namespace
{

enum option
{
    first = 256,
    no_code_in_header,
    debug_checks,
    usage
};

}

parse_command_line_result parse_command_line(int argc, char* argv[], argpppp::pf flags_for_unit_test)
{
    // TODO: parse command line using argpppp
    //       * Should now add callbacks and set values
    // TODO: are the defaults still good? Can/should we calculate them somehow? (no they're not good anymore, since the default preset is now 3)
    // TODO: remove bogus_callback. This is only here to set up the parser
    auto bogus_callback = [](const char*) { return true; };

    // TODO: set up callbacks to fill in command line options
    // TODO: somehow, return options back to caller
    parse_command_line_result result;
    argpppp::parser parser;

    // TODO: argpppp improvements:
    //       * TBH I am not so happy with those set_xxx() methods: why did we call them set_xxx() rather than xxx()?
    //       * Not sure whether it makes sense to have the flags on the parse method: we're already using an object,
    //         so we could put them just as well onto a setter
    //       * It is not immediately obvious what the default value for parser flags would be. Either rename 'none' to 'default',
    //         or, alternatively, have an additional enum member defined as such: 'default = none'
    parser.set_doc(
        SHRINKLER_GBA_PROJECT_NAME " - Shrinkler for the Gameboy Advance by Tom/Vantage\n"
        "Shrinkler compression by Blueberry/Loonies\n"
        "https://github.com/tom42/shrinkler-gba");
    parser.set_args_doc("FILE");
    parser.set_nargs(1);
    add_header(parser, "General options:");
    add_option(parser, { "output-file", 'o', "FILE", {}, "Specify output filename. The default output filename is the input filename with the extension replaced by .gba" }, [&](auto s) { result.options.output_file(s); return true; }); // TODO: can we shorten this? Is it possible to overload stuff such that the return can be omitted if we do not wish to check anything?
    add_option(parser, { "verbose", 'v', {}, {}, "Print verbose messages", 0 }, [&](auto) { result.options.verbose(true); return true; });
    add_header(parser, "Depacker options:");
    add_option(parser, { "no-code-in-header", option::no_code_in_header, {}, {}, "Do not put code in ROM header" }, bogus_callback);
    add_option(parser, { "debug-checks", option::debug_checks, {}, {}, "Add debug checks to depacker code" }, bogus_callback);
    add_header(parser, "Shrinkler compression options (default values in parentheses):");
    add_option(parser, { "same-length", 'a', "N", {}, "Number of matches of the same length to consider (20)" }, bogus_callback);
    add_option(parser, { "effort", 'e', "N", {}, "Perseverance in finding multiple matches (200)" }, bogus_callback);
    add_option(parser, { "iterations", 'i', "N", {}, "Number of iterations for the compression (2)" }, bogus_callback);
    add_option(parser, { "length-margin", 'l', "N", {}, "Number of shorter matches considered for each match (2)" }, bogus_callback);
    add_option(parser, { "preset", 'p', "PRESET", {}, "Preset for all compression options except --references (1..9, default 2)" }, bogus_callback);
    add_option(parser, { "references", 'r', "N", {}, "Number of reference edges to keep in memory (100000)" }, bogus_callback);
    add_option(parser, { "skip-length", 's', "N", {}, "Minimum match length to accept greedily (2000)" }, bogus_callback);

    // TODO: handle case where result.errnum is nonzero: that's some sort of parse error, which requires main() to exit
    //       * zero => we're good
    //       * nonzero => we're not good, do nothing
    //       * The case do nothing and exit with success does not exist, because we're letting argp_parse exit (maybe document that)
    // TODO: UGH: need to think about return value (blah, for sake of simplicty pass it as non-const output argument?)
    auto parse_result = parser.parse(argc, argv, flags_for_unit_test);
    result.success = parse_result.errnum == 0;
    if (parse_result.args.size() > 0)
    {
        result.options.input_file(parse_result.args[0]);
    }

    return result;
}

}
