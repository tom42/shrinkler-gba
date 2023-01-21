// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "shrinklergbacore/command_line.hpp"
#include "shrinklergbacore/gba_packer.hpp"
#include "shrinklergbacore/options.hpp"

using namespace shrinklergbacore;

static void process(const options& options)
{
    gba_packer packer;
    packer.pack(options);
}

int main(int argc, char* argv[])
{
    try
    {
        options options;
        auto action = parse_command_line(argc, argv, options, false);
        switch (action)
        {
            case command_action::exit_failure:
                return EXIT_FAILURE;
            case command_action::exit_success:
                return EXIT_SUCCESS;
            case command_action::process:
                process(options);
                return EXIT_SUCCESS;
            default:
                throw std::runtime_error("Unknown action returned by command line parser");
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << argv[0] << ": " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
