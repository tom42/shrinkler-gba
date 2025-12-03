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
            // TODO: test this branch (there is a commented out test in input_file_test that can be used for this)
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        ELFIO::elfio elfio;
        if (!elfio.load(stream))
        {
            // TODO: test this branch (there is a commented out test in input_file_test that can be used for this)
            throw std::runtime_error("File is not a valid ELF file");
        }

        // TODO: have a dedicated method for this in elf_strings.cppm?
        display_program_headers(elfio, console);
        display_section_headers(elfio, console);

        input_file f(elfio);

        // TODO: check for data size > 0 here

        console.verbose("Entry: {:#x}", f.entry());
        console.verbose("Load address: {:#x}", f.load_address());
        console.verbose("Total size of loaded data: {0:#x} ({0})", f.loaded_data_size());

        return f;
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(path + ": " + e.what());
    }

    // TODO: reimplement this: we do file IO here. We then log elf info and then we load the input file
    /*
    auto f = input_file::load(path.string(), console);

    if (!f.loaded_data_size())
    {
        // Shrinkler does really not like files with size zero.
        // TODO: what about agbpack (clownlzss + huffman; what's the minimum size they would like to have?)
        throw std::runtime_error("File is too small to be compressed");
    }

    return f;
    */
}

}

void pack(const options& opts)
{
    auto console = create_console(opts);
    auto input_file = load_input_file(opts.input_file().string(), console);

    // TODO: implement the remaining 5/6/whatever steps (see old implementation):
    //       * Compress raw binary: note: here some work on libshrinkler might be necessary: I am not sure it supports progress output. But that's fine.
    //       * Assemble cart
    //       * Fix up for EZFlash (why is that not done by the assembler? => because that's post processing and needs to be done for both assemblers?)
    //       * Write result to disk
    //       * Verbose output of final cart sizes
}

}
