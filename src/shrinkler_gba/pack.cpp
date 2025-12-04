// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <elfio/elfio.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <system_error>

module shrinkler_gba;
import :input_file;
import libshrinkler;

namespace shrinkler_gba
{

namespace
{

console create_console(const options& opts)
{
    console c;

    if (!opts.verbose())
    {
        c.verbose_stream(nullptr);
    }

    return c;
}

input_file load_input_file(const std::string& path, const console& console)
{
    try
    {
        console.verbose("Loading: {}", path);
        std::ifstream stream(path, std::ios::binary);
        if (!stream)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        ELFIO::elfio elfio;
        if (!elfio.load(stream))
        {
            throw std::runtime_error("File is not a valid ELF file");
        }

        display_elf_info(elfio, console);

        input_file f(elfio);

        console.verbose("Entry: {:#x}", f.entry());
        console.verbose("Load address: {:#x}", f.load_address());
        console.verbose("Total size of loaded data: {0:#x} ({0})", f.loaded_data_size());

        if (!f.loaded_data_size())
        {
            // Shrinkler does really not like files with size zero.
            // TODO: what about agbpack (clownlzss + huffman; what's the minimum size they would like to have?)
            throw std::runtime_error("File is too small to be compressed");
        }

        return f;
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(path + ": " + e.what());
    }
}

std::vector<unsigned char> compress(const std::vector<unsigned char>& uncompressed_binary, const options& opts)
{
    // TODO: encode (get compressed data + compression info)
    // TODO: dump compression info
    // TODO: progress output for libshrinkler?
    // TODO: check parameters
    //       * What about the parity context? (Well we can measure whether messing with it yields smaller binaries, but as I understand it also needs a different packer)
    //       * Endianness: this is currently set to 'big', which is of course not what we want => best to set the endianness to big here
    libshrinkler::encoder encoder;
    encoder.parameters(opts.shrinkler_parameters());
    auto compressed_binary = encoder.encode(uncompressed_binary);
    return compressed_binary;
}

}

void pack(const options& opts)
{
    auto console = create_console(opts);
    auto input_file = load_input_file(opts.input_file().string(), console);
    // TODO: currently we only do shrinkler compression. Later we'll also support lzss+huffman compression
    auto compressed_binary = compress(input_file.data(), opts);

    // TODO: implement the remaining 5/6/whatever steps (see old implementation):
    //       * Compress raw binary: note: here some work on libshrinkler might be necessary: I am not sure it supports progress output. But that's fine.
    //       * Assemble cart
    //       * Fix up for EZFlash (why is that not done by the assembler? => because that's post processing and needs to be done for both assemblers?)
    //       * Write result to disk
    //       * Verbose output of final cart sizes
}

}
