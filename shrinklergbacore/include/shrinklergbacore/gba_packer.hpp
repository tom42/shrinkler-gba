// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_GBA_PACKER_HPP
#define SHRINKLERGBACORE_GBA_PACKER_HPP

#include <filesystem>
#include <vector>
#include "shrinklergbacore/options.hpp"

namespace shrinklergbacore
{

class input_file;

class gba_packer final
{
public:
    void pack(const options& options);
private:
    void write_to_disk(const std::vector<unsigned char>& data, const std::filesystem::path& filename);
    void remove_output_file(const std::filesystem::path& filename);
};

}

#endif
