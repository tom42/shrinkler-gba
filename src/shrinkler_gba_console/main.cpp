// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <cstdlib>
#include <iostream>
#include <stdexcept>

import shrinkler_gba;

int main(int argc, char* argv[])
{
    try
    {
        // TODO: parse command line, then
        //       * Either run the packer
        //       * Do nothing and exit with error (orly? Aren't we letting argp_parse exit?)
        //       * Do nothing and exit with error (orly? Aren't we letting argp_parse exit?)
        shrinkler_gba::parse_command_line(argc, argv);
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << argv[0] << ": " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
