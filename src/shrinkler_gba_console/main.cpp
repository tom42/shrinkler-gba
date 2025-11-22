// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <cstdlib>
#include <iostream>
#include <stdexcept>

import shrinkler_gba;

namespace
{

void run(const shrinkler_gba::options& /*options*/)
{
    // TODO: create and run packer
}

}

int main(int argc, char* argv[])
{
    try
    {
        auto result = shrinkler_gba::parse_command_line(argc, argv);
        if (!result.success)
        {
            // Should not happen because we let argp_parse exit.
            return EXIT_FAILURE;
        }

        run(result.opts);
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << argv[0] << ": " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
