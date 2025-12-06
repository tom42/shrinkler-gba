// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <cassert>
#include <cerrno>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>
#include "shrinklerwrapper/shrinklerwrapper.hpp"
#include "shrinklergbacore/cart_assembler.hpp"
#include "shrinklergbacore/console.hpp"
#include "shrinklergbacore/gba_packer.hpp"
#include "shrinklergbacore/input_file.hpp"

namespace shrinklergbacore
{

void gba_packer::pack(const options& options)
{
    CONSOLE_VERBOSE(console) << "Writing: " << options.output_file().string() << std::endl;
    write_to_disk(cart_data, options.output_file());
}

}
