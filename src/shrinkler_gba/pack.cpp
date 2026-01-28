// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cerrno>
#include <elfio/elfio.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <gsl/gsl>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

module shrinkler_gba;
import :input_file;
import :utility;
import agbpack;
import libshrinkler;

namespace shrinkler_gba
{

namespace fs = std::filesystem;

namespace
{

console create_console(const options& opts)
{
    console c;

    if (!opts.verbose)
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
            // agbpack LZSS can compress files with size zero. The output for such a file is 4 bytes,
            // which in turn can be processed by the huffman encoder.
            throw std::runtime_error("File is too small to be compressed");
        }

        return input_file;
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(path + ": " + e.what());
    }
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

void display_sizes(const cartridge& cartridge, const console& console)
{
    console.verbose("Packer                : {}", cartridge.packer);
    console.verbose("Compressed binary size: {:4} bytes", cartridge.compressed_size);
    console.verbose("Depacker size         : {:4} bytes (excluding code in cartridge header)", cartridge.depacker_size);
    console.verbose("Cartridge size        : {:4} bytes", cartridge.data.size());

    for (const auto& info : cartridge.info)
    {
        console.out("Note: {}", info);
    }
}

std::unique_ptr<packer> get_packer(std::string_view name)
{
    const packer_info* pi = packer_registry::find_info(name);
    if (!pi)
    {
        throw internal_error(std::format("Unknown packer: '{}'", name));
    }

    return pi->create();
}

std::vector<std::unique_ptr<packer>> get_packers(const options& opts)
{
    if (opts.packer.has_value())
    {
        std::vector<std::unique_ptr<packer>> p;
        p.push_back(get_packer(*opts.packer));
        return p;
    }

    return packer_registry::create_all_packers();
}

auto smallest_cartridge(const std::vector<cartridge>& cartridges)
{
    // Return iterator to avoid unnecessary copy and to have the ability of error handling.
    return std::ranges::min_element(cartridges, {}, [](const auto& cartridge) { return cartridge.data.size(); });
}

cartridge try_all_packers(const input_file& input_file, const options& opts, const console& console)
{
    std::vector<cartridge> cartridges;

    for (const auto& packer : get_packers(opts))
    {
        cartridges.push_back(packer->pack(input_file, opts));
        fix_cartridge_for_ezf_advance(cartridges.back().data, console);
        display_sizes(cartridges.back() , console);
    }

    auto smallest = smallest_cartridge(cartridges);
    if (smallest == cartridges.end())
    {
        throw std::runtime_error("No packer could compress the input");
    }

    return *smallest;
}

void remove_output_file(const fs::path& filename)
{
    // Delete file, ignore any error. Use overload of remove that does not throw.
    std::error_code e;
    fs::remove(filename, e);
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

}

void pack(const options& opts)
{
    auto console = create_console(opts);
    auto input_file = load_input_file(opts.input_file.string(), console);

    // TODO: honor --packer option
    //       * If only one packer is specified, adapt error message if compression failed (if packer gave up?)
    auto cartridge = try_all_packers(input_file, opts, console);

    console.verbose("Smallest cartridge is produced by {} ({} bytes)", cartridge.packer, cartridge.data.size());
    write_to_disk(cartridge.data, opts.output_file.string(), console);
}

}
