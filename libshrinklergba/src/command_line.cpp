// MIT License
//
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
// Copyright (c) 2021 Thomas Mathys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <argp.h> // TODO: how to include it? "argp.h"? <argp.h>?
#include "libshrinklergba/command_line.hpp"

// TODO: get this from header file
#define PROJECT_NAME "shrinkler-gba"

namespace libshrinklergba
{

enum option
{
    first = 256,
    usage
};

static error_t parse_opt(int key, char* arg, argp_state* state) noexcept
{
    try
    {
        parser* p = static_cast<parser*>(state->input);
        return p->parse_opt(key, arg, state);
    }
    catch (const std::exception& e)
    {
        // Do not let any exception escape into argp, which is written in C.
        argp_error(state, "%s", e.what());
        return EINVAL;
    }
}

command_action parse_command_line(int /*argc*/, char* /*argv*/[], options& /*options*/, bool /*silent*/)
{
    static const char doc[] =
        PROJECT_NAME " - Gameboy Advance Intro Cruncher by Tom/Vantage\n"   // TODO: fix name
        "Uses Shrinkler compression by Blueberry\n"                         // TODO: is this still needed in that form?
        "https://github.com/tom42/gbaic";                                   // TODO: fix URL
    static const char args_doc[] = "FILE";

    static const argp_option argp_options[] =
    {
        { 0, 0, 0, 0, "General options:", 0 },
        { "output-file", 'o', "FILE", 0, "Specify output filename. The default output filename is the input filename with the extension replaced by .gba", 0 },
        { "verbose", 'v', 0, 0, "Print verbose messages", 0 },

        // Shrinkler compression options
        { 0, 0, 0, 0, "Shrinkler compression options (default values in parentheses):", 0 },
        { "same-length", 'a', "N", 0, "Number of matches of the same length to consider (20)", 0 },
        { "effort", 'e', "N", 0, "Perseverance in finding multiple matches (200)", 0 },
        { "iterations", 'i', "N", 0, "Number of iterations for the compression (2)", 0 },
        { "length-margin", 'l', "N", 0, "Number of shorter matches considered for each match (2)", 0 },
        { "preset", 'p', "PRESET", 0, "Preset for all compression options except --references (1..9, default 2)", 0 },
        { "references", 'r', "N", 0, "Number of reference edges to keep in memory (100000)", 0 },
        { "skip-length", 's', "N", 0, "Minimum match length to accept greedily (2000)", 0 },

        // argp always forces "help" and "version" into group -1, but not "usage".
        // But we want "usage" to be there too, so we explicitly specify -1 for "help".
        // That way, "version" and "usage" will inherit it.
        { "help", '?', 0, 0, "Give this help list", -1 },
        { "version", 'V', 0, 0, "Print program version", 0},
        { "usage", option::usage, 0, 0, "Give a short usage message", 0 },
        { 0, 0, 0, 0, 0, 0 }
    };

    static const argp argp = { argp_options, parse_opt, args_doc, doc, 0, 0, 0 };

    // TODO: real implementation
    return command_action::exit_failure;
}

}
