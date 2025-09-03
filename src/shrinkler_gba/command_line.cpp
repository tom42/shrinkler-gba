// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module shrinkler_gba;
import argpppp;

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

void parse_command_line(int argc, char* argv[])
{
    // TODO: parse command line using argpppp
    //       * Should now add callbacks
    // TODO: add version
    //       * First of all need a version header
    //       * Then we need to define argp_program_version
    // TODO: do we add a bug report address?
    // TODO: are the defaults still good? Can/should we calculate them somehow?
    // TODO: remove bogus_callback. This is only here to set up the parser
    auto bogus_callback = [](const char*) { return true; };

    argpppp::parser parser;
    add_header(parser, "General options:");
    add_option(parser, { "output-file", 'o', "FILE", {}, "Specify output filename. The default output filename is the input filename with the extension replaced by .gba" }, bogus_callback);
    add_option(parser, { "verbose", 'v', {}, {}, "Print verbose messages", 0 }, bogus_callback);
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
    parser.parse(argc, argv);
}

}
