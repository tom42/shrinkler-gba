// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <cassert>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>
#include "fmt/core.h"
#include "shrinklerwrapper/shrinklerwrapper.hpp"
#include "shrinklergbacore/cart_assembler.hpp"
#include "shrinklergbacore/console.hpp"
#include "shrinklergbacore/gba_packer.hpp"
#include "shrinklergbacore/input_file.hpp"

namespace shrinklergbacore
{

void gba_packer::pack(const options& options)
{
    console console;
    console.verbose(options.verbose() ? &std::cout : nullptr);

    // Load program
    input_file input_file(console);
    input_file.load(options.input_file());

    // Compress program
    shrinklerwrapper::shrinkler_compressor compressor;
    compressor.set_parameters(options.shrinkler_parameters());
    auto compressed_program = compressor.compress(input_file.data());

    // Assemble cart
    const depacker_settings depacker_settings
    {
        .code_in_header = options.code_in_header(),
        .debug_checks = options.debug_checks()
    };
    cart_assembler cart_assembler(input_file, compressed_program, depacker_settings);
    std::vector<unsigned char> cart_data = cart_assembler.data();

    // EZF Advance removes trailing 0xff bytes.
    // If the last byte is 0xff, pad the image so that nothing important is removed.
    if (cart_data.size() && (cart_data.back() == 0xff))
    {
        cart_data.push_back('T');
        cart_data.push_back('o');
        cart_data.push_back('m');
        cart_data.push_back('!');
        CONSOLE_WARN(console) << "Last byte of cart was 0xff. Appended padding word to protect against EZF Advance" << std::endl;
    }

    CONSOLE_VERBOSE(console) << fmt::format("Uncompressed data size: {:4} bytes", input_file.data().size()) << std::endl;
    CONSOLE_VERBOSE(console) << fmt::format("Compressed data size  : {:4} bytes", compressed_program.size()) << std::endl;
    CONSOLE_VERBOSE(console) << fmt::format("Depacker size         : {:4} bytes (excluding code in cartridge header)", cart_assembler.depacker_size()) << std::endl;
    CONSOLE_VERBOSE(console) << fmt::format("Cartridge size        : {:4} bytes", cart_data.size()) << std::endl;
    CONSOLE_VERBOSE(console) << "Writing: " << options.output_file().string() << std::endl;
    write_to_disk(cart_data, options.output_file());
}

void gba_packer::write_to_disk(const std::vector<unsigned char>& data, const std::filesystem::path& filename)
{
    try
    {
        std::ofstream file;
        file.open(filename.string(), std::ios::binary | std::ios::trunc);
        if (!file)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        if (!file)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        file.close();
    }
    catch (const std::system_error& e)
    {
        remove_output_file(filename);
        throw std::runtime_error(fmt::format("could not write {}: {}", filename.string(), e.what()));
    }
}

void gba_packer::remove_output_file(const std::filesystem::path& filename)
{
    // Delete file, ignore any error. Use overload of remove that does not throw.
    std::error_code e;
    std::filesystem::remove(filename, e);
}

}
