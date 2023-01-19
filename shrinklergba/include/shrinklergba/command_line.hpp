// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBA_COMMAND_LINE_HPP
#define SHRINKLERGBA_COMMAND_LINE_HPP

#include "shrinklergba/options.hpp"

namespace shrinklergba
{

enum class command_action
{
    exit_failure,
    exit_success,
    process
};

command_action parse_command_line(int argc, char* argv[], options& options, bool silent);

}

#endif
