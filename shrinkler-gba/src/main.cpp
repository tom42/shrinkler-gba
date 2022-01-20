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

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "shrinklergba/command_line.hpp"
#include "shrinklergba/gba_packer.hpp"
#include "shrinklergba/options.hpp"

using namespace shrinklergba;

static void process(const options& /*options*/)
{
    // TODO: process input file
    //       * Set options
    //       * Tell the thing to do its work
    //gba_packer packer;
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
