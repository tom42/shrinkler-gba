// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

export module shrinkler_gba:command_line;
import argpppp;
import :options;

namespace shrinkler_gba
{

export struct parse_command_line_result final
{
    bool success{};
    options options{};
};

export parse_command_line_result parse_command_line(int argc, char* argv[], argpppp::pf flags_for_unit_test = argpppp::pf::none);

}
