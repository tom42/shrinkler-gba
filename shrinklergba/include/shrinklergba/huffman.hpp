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

// Description of Huffman encoded data as understood by the GBA BIOS, from gbatek:
//
// Data Header (32 bit)
//   Bit 0-3    Data size in bit units (normally 4 or 8)
//   Bit 4-7    Compressed type (must be 2 for Huffman)
//   Bit 8-31   24 bit size of decompressed data in bytes
// Tree size (8 bit)
//   Bit 0-7    Size of tree table / 2 - 1 (ie. offset to compressed bitstream)
// Tree table (list of 8 bit nodes, starting with the root node)
//   Root node and non-data child nodes are:
//   Bit 0-5    Offset to next child node,
//              Next child node0 is at (CurrentAddr AND NOT 1) + Offset * 2 + 2
//              Next child node1 is at (CurrentAddr AND NOT 1) + Offset * 2 + 2 + 1
//   Bit 6      Node1 end flag (1 = next child node is data)
//   Bit 7      Node0 end flag (1 = next child node is data)
// Data nodes are (when end flag was set in parent node):
//   Bit 0-7    Data (upper bits should be zero if data size is less than 8)
// Compressed bitstream (stored in units of 32bits)
//   Bit 0-31   Node bits (bit 31 = first bit) (0=node0, 1=node1)

#ifndef SHRINKLERGBA_HUFFMAN_HPP
#define SHRINKLERGBA_HUFFMAN_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

namespace shrinklergba
{

class huffman_decoder
{
public:
    std::vector<unsigned char> decode(const std::vector<unsigned char>& compressed_data);
    std::vector<unsigned char> decode(const unsigned char* compressed_data, std::size_t size);
private:
    unsigned char decode_symbol();
    bool get_bit();

    static int check_symbol_size(int symbol_size);
    static std::size_t get_decompressed_size(const unsigned char* compressed_data);

    const unsigned char* tree_size;
    const uint32_t* readptr;
    int symbol_mask;
    uint32_t bitbuffer;
    uint32_t bitmask;
};

class huffman_encoder
{
public:
    std::vector<unsigned char> encode(const std::vector<unsigned char>& data);
    std::vector<unsigned char> encode(const unsigned char* data, std::size_t size);
private:
};

}

#endif
