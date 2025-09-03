// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module shrinkler_gba;
import argpppp;

namespace shrinkler_gba
{

void parse_command_line(int argc, char* argv[])
{
    // TODO: parse command line using argpppp
    argpppp::parser parser;
    parser.parse(argc, argv);
}

}
