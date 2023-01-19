// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <stdexcept>
#include "shrinklergba/gba_bios_common.hpp"

namespace shrinklergba
{

template <typename E>
constexpr auto to_underlying(E enumerator) noexcept
{
    return static_cast<std::underlying_type_t<E>>(enumerator);
}

void throw_if_wrong_compressed_size(std::size_t compressed_size)
{
    // Check the bare minimum size required for the compression header.
    // In reality things are more complicated. If the decompressed size is 0, then 4 bytes is enough.
    // If the decompressed size is nonzero, then the minimum size is more than 4 bytes and may
    // depend on various additional things, e.g. the tree size in the case of Huffman encoding.
    if (compressed_size < 4)
    {
        throw std::runtime_error("compressed data is too short");
    }

    if (compressed_size % 4 != 0)
    {
        throw std::runtime_error("compressed data size is not a multiple of 4 bytes");
    }
}

void throw_if_wrong_compression_type(compression_type expected, int actual)
{
    if (to_underlying(expected) != actual)
    {
        throw std::runtime_error("invalid compression type");
    }
}

std::size_t get_decompressed_size(const unsigned char* compressed_data)
{
    unsigned char b0 = compressed_data[ofs_decompressed_size + 0];
    unsigned char b1 = compressed_data[ofs_decompressed_size + 1];
    unsigned char b2 = compressed_data[ofs_decompressed_size + 2];

    auto size = b0 + (b1 << 8) + (b2 << 16);
    return size;
}

}
