// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cerrno>
#include <elfio/elfio.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <gsl/gsl>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

module shrinkler_gba;
import :input_file;
import agbpack;
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

bytevector shrinkler_compress(const bytevector& uncompressed_binary, const options& opts)
{
    // TODO: dump compression info (whatever it is that encoder can spit out additionally and that we might want to output - the references warning thing, mostly)
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

cartridge assemble_shrinkler_cartridge(const input_file& input_file, const bytevector& compressed_binary, const options& opts)
{
    // TODO: later we'll have multiple algorithms, but for the time being that's fine
    //       * Note that since the total size is given by cart header + depacker + packed program this means that both compress() and assemble_cartridge() possibly need to go into some sort of class
    //       * Can we compare the output we're getting here against an old version? Should be bit for bit the same, no?
    const shrinkler_packer_options options
    {
        .code_in_header = opts.code_in_header(),
        .debug_checks = opts.debug_checks()
    };
    shrinkler_cartridge_assembler assembler(input_file, compressed_binary, options);

    return cartridge
    {
        .data = assembler.data(),
        .compressed_size = compressed_binary.size(),
        .depacker_size = assembler.depacker_size()
    };
}

cartridge pack_shrinkler(const input_file& input_file, const options& opts)
{
    auto compressed_binary = shrinkler_compress(input_file.data(), opts);
    return assemble_shrinkler_cartridge(input_file, compressed_binary, opts);
}

void fix_cartridge_for_ezf_advance(bytevector& cartridge_data, const console& console)
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

void remove_output_file(const std::filesystem::path& filename)
{
    // Delete file, ignore any error. Use overload of remove that does not throw.
    std::error_code e;
    std::filesystem::remove(filename, e);
}

void write_to_disk(const bytevector& data, const std::string& filename, const console& console)
{
    try
    {
        console.verbose("Writing: {}", filename);

        std::ofstream file;
        file.open(filename, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        file.write(reinterpret_cast<const char*>(data.data()), gsl::narrow<std::streamsize>(data.size()));
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
        throw std::runtime_error(std::format("Could not write {}: {}", filename, e.what()));
    }
}

void display_sizes(const input_file& input_file, const cartridge& cartridge, const console& console)
{
    console.verbose("Uncompressed binary size: {:4} bytes", input_file.data().size());
    console.verbose("Compressed binary size  : {:4} bytes", cartridge.compressed_size);
    console.verbose("Depacker size           : {:4} bytes (excluding code in cartridge header)", cartridge.depacker_size);
    console.verbose("Cartridge size          : {:4} bytes", cartridge.data.size());
}

}

void pack(const options& opts)
{
    auto console = create_console(opts);
    auto input_file = load_input_file(opts.input_file().string(), console);

    // TODO: currently we only do shrinkler compression. Later we'll also support lzss+huffman compression
    //       => This must somehow be abstracted
    //       => Note that the cartridge fix must be done individually, since different compression algos may yield different compressed data
    //          => Huh? What exactly do I mean? The thing is, this can be done in a generic way because it is the same for all packers,
    //             but otoh we only know *after* assembling whether the fixing up is necessary. This may or may not increase the final
    //             cartridge size, so it must be done *before* choosing the smallest cartridge
    //             => So:
    //             * For each packer
    //               * Assemble cart
    //               * Fix cart for EZF Advance (may or may not be necessary)
    //             * Choose smallest cart
    auto cartridge = pack_shrinkler(input_file, opts);
    fix_cartridge_for_ezf_advance(cartridge.data, console);

    write_to_disk(cartridge.data, opts.output_file().string(), console);
    display_sizes(input_file, cartridge, console);

    // TODO: test code: LZSS. What we really want to do is: we want to try a number of methods and then choose the best one
    // TODO: have a similar class for shrinkler: shrinkler_packer
    //       * Turn shrinkler_cartridge_assembler.cpp(m) into shrinkler_packer.cpp(m)
    //       * shrinkler_cartridge_assembler will then be private to shrinkler_packer.cpp and be in an anonymous namespace
    lzss_packer_options options
    {
        .code_in_header = opts.code_in_header(),
    };
    lzss_packer lzss_packer(options);
    auto cartridge2 = lzss_packer.pack(input_file);
    write_to_disk(cartridge2.data, opts.output_file().string(), console);
    display_sizes(input_file, cartridge2, console);
}

}
