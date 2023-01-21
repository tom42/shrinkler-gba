// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_GBA_BIOS_COMMON_HPP
#define SHRINKLERGBACORE_GBA_BIOS_COMMON_HPP

#include <cstddef>

namespace shrinklergbacore
{

enum class compression_type : unsigned char
{
    lzss = 1,
    huffman = 2
};

// Offsets relative to start of header
constexpr std::size_t ofs_compression_type = 0;
constexpr std::size_t ofs_decompressed_size = 1;

void throw_if_wrong_compressed_size(std::size_t compressed_size);
void throw_if_wrong_compression_type(compression_type expected, int actual);
std::size_t get_decompressed_size(const unsigned char* compressed_data);

}

#endif
