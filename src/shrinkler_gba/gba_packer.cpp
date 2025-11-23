// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <filesystem>

module shrinkler_gba;

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

input_file load_input_file(const std::filesystem::path& path, const console& console)
{
    input_file f;
    f.load(path.string(), console);

    if (!f.loaded_data_size())
    {
        // Shrinkler does really not like files with size zero.
        // TODO: what about agbpack (clownlzss + huffman; what's the minimum size they would like to have?)
        throw std::runtime_error("File is too small to be compressed");
    }

    return f;
}

}

void pack(const options& opts)
{
    auto console = create_console(opts);
    auto input_file = load_input_file(opts.input_file(), console);

    // TODO: implement the existing 5/6/whatever steps:
    //       * Compress raw binary: note: here some work on libshrinkler might be necessary: I am not sure it supports progress output. But that's fine.
    //       * Assemble cart
    //       * Fix up for EZFlash (why is that not done by the assembler? => because that's post processing and needs to be done for both assemblers?)
    //       * Write result to disk
    // TODO: here we would like to rework the existing components, so that they do NOT produce console output. All console output should be controlled/done up here in this file
}

// TODO: old stuff below (packer-as-a-class: probably delete this once we do not need it anymore)
void gba_packer::pack(const options&)
{
    initialize_console_output();
    load_input_file();
}

void gba_packer::initialize_console_output()
{
    // TODO: initialize console output
}

void gba_packer::load_input_file()
{
    // TODO: basically, load input file
}

}
