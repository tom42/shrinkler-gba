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
        case option::no_code_in_header:
            m_options.code_in_header(false);
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
