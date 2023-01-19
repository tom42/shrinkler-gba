// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBA_GBA_PACKER_HPP
#define SHRINKLERGBA_GBA_PACKER_HPP

#include <filesystem>
#include <vector>
#include "shrinklergba/options.hpp"

namespace shrinklergba
{

class input_file;

class gba_packer final
{
public:
    void pack(const options& options);
private:
    void write_complement(std::vector<unsigned char>& cart);
    void write_to_disk(const std::vector<unsigned char>& data, const std::filesystem::path& filename);
    void remove_output_file(const std::filesystem::path& filename);
    std::vector<unsigned char> make_shrinklered_cart(const input_file& input_file, const options& options);
};

}

#endif
