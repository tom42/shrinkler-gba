// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <argp.h>
#include "shrinklerwrapper/shrinklerwrapper.hpp"
#include "shrinklergbacore/command_line.hpp"
#include "shrinklergbacore_version.hpp"

namespace shrinklergbacore
{

enum option
{
    first = 256,
    debug_checks,
    usage
};

class parser final
{
public:
    parser(options& options, bool silent) : m_options(options), m_silent(silent) {}

    error_t parse_opt(int key, char* arg, argp_state* state)
    {
        switch (key)
        {
        case 'o':
            m_options.output_file(arg);
            return 0;
        case 'v':
            m_options.verbose(true);
            m_options.shrinkler_parameters().verbose = true;
            return 0;
        case option::debug_checks:
            m_options.debug_checks(true);
            return 0;
        case 'a':
            return parse_int("same length count", arg, 1, 100000, state, m_options.shrinkler_parameters().same_length);
        case 'e':
            return parse_int("effort", arg, 0, 100000, state, m_options.shrinkler_parameters().effort);
        case 'i':
            return parse_int("number of iterations", arg, 1, 9, state, m_options.shrinkler_parameters().iterations);
        case 'l':
            return parse_int("length margin", arg, 0, 100, state, m_options.shrinkler_parameters().length_margin);
        case 'p':
            return parse_preset(arg, state);
        case 'r':
            return parse_int("number of references", arg, 1000, 100000000, state, m_options.shrinkler_parameters().references);
        case 's':
            return parse_int("skip length", arg, 2, 100000, state, m_options.shrinkler_parameters().skip_length);
        case '?':
            argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            stop_parsing_and_exit(state);
            return 0;
        case 'V':
            print_version();
            stop_parsing_and_exit(state);
            return 0;
        case option::usage:
            argp_state_help(state, stdout, ARGP_HELP_USAGE);
            stop_parsing_and_exit(state);
            return 0;
        case ARGP_KEY_ARG:
            if (!m_inputfile_seen)
            {
                m_inputfile_seen = true;
                m_options.input_file(arg);
                return 0;
            }
            else
            {
                argp_error(state, "more than one input file given");
                return EINVAL;
            }
        case ARGP_KEY_NO_ARGS:
            if (m_action != command_action::exit_success)
            {
                argp_error(state, "no input file given");
                return EINVAL;
            }
            else
            {
                return ARGP_ERR_UNKNOWN;
            }
        default:
            return ARGP_ERR_UNKNOWN;
        }
    }

    command_action action() const { return m_action; }

private:
    void stop_parsing_and_exit(argp_state* state)
    {
        state->next = state->argc;
        m_action = command_action::exit_success;
    }

    void print_version()
    {
        if (!m_silent)
        {
            std::cout << SHRINKLERGBA_PROJECT_NAME << " " << SHRINKLERGBA_PROJECT_VERSION << std::endl;
        }
    }

    int parse_preset(const char* s, const argp_state* state)
    {
        int preset = 0;
        auto parse_result = parse_int("preset", s, 1, 9, state, preset);

        if (!parse_result)
        {
            m_options.shrinkler_parameters().preset(preset);
        }

        return parse_result;
    }

    static int parse_int(const char* value_description, const char* s, int min, int max, const argp_state* state, int& parsed_int)
    {
        char* end;
        auto value = strtol(s, &end, 10);

        if ((*end) || (value < min) || (value > max))
        {
            // Exit code (EXIT_FAILURE) is not really used here since we don't allow argp to exit the program.
            // But if we did then that's what we wanted to have as exit code.
            argp_failure(state, EXIT_FAILURE, 0, "invalid %s: %s", value_description, s);
            return EINVAL;
        }

        parsed_int = value;
        return 0;
    }

    command_action m_action = command_action::process;
    bool m_inputfile_seen = false;
    options& m_options;
    const bool m_silent;
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

command_action parse_command_line(int argc, char* argv[], options& options, bool silent)
{
    static const char doc[] =
        SHRINKLERGBA_PROJECT_NAME " - Shrinkler for the Gameboy Advance by Tom/Vantage\n"
        "Shrinkler compression by Blueberry/Loonies\n"
        "https://github.com/tom42/shrinkler-gba";
    static const char args_doc[] = "FILE";

    static const argp_option argp_options[] =
    {
        { 0, 0, 0, 0, "General options:", 0 },
        { "output-file", 'o', "FILE", 0, "Specify output filename. The default output filename is the input filename with the extension replaced by .gba", 0 },
        { "verbose", 'v', 0, 0, "Print verbose messages", 0 },

        // Code generation options
        { 0, 0, 0, 0, "Code generation options:", 0 },
        { "debug-checks", option::debug_checks, 0, 0, "Add debug checks to depacker code", 0},

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

    parser parser(options, silent);

    unsigned int flags = ARGP_NO_EXIT | ARGP_NO_HELP;
    if (silent)
    {
        flags |= ARGP_NO_ERRS;
    }

    if (argp_parse(&argp, argc, argv, flags, 0, &parser))
    {
        return command_action::exit_failure;
    }

    return parser.action();
}

}
