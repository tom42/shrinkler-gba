// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_COMMAND_LINE_HPP
#define SHRINKLERGBACORE_COMMAND_LINE_HPP

#include "shrinklergbacore/options.hpp"

namespace shrinklergbacore
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
