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

// Description of LZSS encoded data as understood by the GBA BIOS, from gbatek:
//
// Data header (32bit)
//   Bit 0-3    Reserved (0)
//   Bit 4-7    Compressed type (must be 1 for LZ77)
//   Bit 8-31   Size of decompressed data
// Repeat below. Each Flag Byte followed by eight Blocks.
// Flag data (8bit)
//   Bit 0-7    Type Flags for next 8 Blocks, MSB first
// Block Type 0 - Uncompressed - Copy 1 Byte from Source to Dest
//   Bit 0-7    One data byte to be copied to dest
// Block Type 1 - Compressed - Copy N+3 Bytes from Dest-Disp-1 to Dest
//   Bit 0-3    Disp MSBs
//   Bit 4-7    Number of bytes to copy (minus 3)
//   Bit 8-15   Disp LSBs

#ifndef SHRINKLERGBA_LZSS_HPP
#define SHRINKLERGBA_LZSS_HPP

#include <cstddef>
#include <vector>

namespace shrinklergba
{

// TODO: add LZSS decoder, with interface similar as in huffman.hpp
class lzss_decoder
{
public:
    std::vector<unsigned char> decode(const std::vector<unsigned char>& compressed_data);
    std::vector<unsigned char> decode(const unsigned char* compressed_data, std::size_t size);
private:
};

}

#endif
