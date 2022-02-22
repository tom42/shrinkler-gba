// MIT License
//
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
// Copyright (c) 2021 Thomas Mathys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

}
