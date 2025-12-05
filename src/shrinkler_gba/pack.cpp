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

        input_file input_file(elfio);

        console.verbose("Entry: {:#x}", input_file.entry());
        console.verbose("Load address: {:#x}", input_file.load_address());
        console.verbose("Total size of loaded data: {0:#x} ({0})", input_file.loaded_data_size());

        if (!input_file.loaded_data_size())
        {
            // Shrinkler does really not like files with size zero.
            // TODO: what about agbpack (clownlzss + huffman; what's the minimum size they would like to have?)
            throw std::runtime_error("File is too small to be compressed");
        }

        return input_file;
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
    // TODO: progress output for libshrinkler? => Well what is not implemented inside libshrinkler
    using namespace libshrinkler;

    encoder_parameters encoder_parameters(opts.shrinkler_parameters());
    encoder_parameters.endianness(endianness::little);

    // Note: we could experiment with disabling the parity context,
    // but so far I have not seen a binary where that yielded better compression.
    // Moreover we'd have to use an alternate depacker.
    encoder encoder;
    encoder.parameters(encoder_parameters);

    auto compressed_binary = encoder.encode(uncompressed_binary);
    return compressed_binary;
}

cartridge assemble_cartridge(const input_file& input_file, const std::vector<unsigned char>& compressed_binary, const options& opts)
{
    // TODO: later we'll have multiple algorithms, but for the time being that's fine
    //       * Note that since the total size is given by cart header + depacker + packed program this means that both compress() and assemble_cartridge() possibly need to go into some sort of class
    //       * Can we compare the output we're getting here against an old version? Should be bit for bit the same, no?
    const shrinkler_depacker_settings depacker_settings
    {
        .code_in_header = opts.code_in_header(),
        .debug_checks = opts.debug_checks()
    };
    shrinkler_cartridge_assembler assembler(input_file, compressed_binary, depacker_settings);

    return cartridge
    {
        .data = assembler.data(),
        .depacker_size = assembler.depacker_size()
    };
}

void fix_cartridge_for_ezf_advance(std::vector<unsigned char>& cartridge_data, const console& console)
{
    // EZF Advance removes trailing 0xff bytes.
    // If the last byte is 0xff, pad the image so that nothing important is removed.
    if (cartridge_data.size() && (cartridge_data.back() == 0xff))
    {
        cartridge_data.push_back('T');
        cartridge_data.push_back('o');
        cartridge_data.push_back('m');
        cartridge_data.push_back('!');
        console.warn("Last byte of cart was 0xff. Appended padding word to protect against EZF Advance");
    }
}

}

void pack(const options& opts)
{
    auto console = create_console(opts);
    auto input_file = load_input_file(opts.input_file().string(), console);
    // TODO: currently we only do shrinkler compression. Later we'll also support lzss+huffman compression
    auto compressed_binary = compress(input_file.data(), opts);
    auto cartridge = assemble_cartridge(input_file, compressed_binary, opts);
    fix_cartridge_for_ezf_advance(cartridge.data, console);

    console.verbose("Uncompressed size: {:4} bytes", input_file.data().size());
    console.verbose("Compressed size  : {:4} bytes", compressed_binary.size());
    console.verbose("Depacker size    : {:4} bytes (excluding code in cartridge header)", cartridge.depacker_size);
    console.verbose("Cartridge size   : {:4} bytes", cartridge.data.size());

    // TODO: implement the remaining 5/6/whatever steps (see old implementation):
    //       * Write result to disk
}

}
