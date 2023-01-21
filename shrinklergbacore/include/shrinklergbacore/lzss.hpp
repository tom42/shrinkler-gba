// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

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
//
// "If the size of the compressed data is not a multiple of 4, please adjust it
// as much as possible by padding with 0. Align the source address to a 4-Byte
// boundary."

#ifndef SHRINKLERGBACORE_LZSS_HPP
#define SHRINKLERGBACORE_LZSS_HPP

#include <cstddef>
#include <vector>

namespace shrinklergbacore
{

class lzss_decoder
{
public:
    std::vector<unsigned char> decode(const std::vector<unsigned char>& compressed_data);
    std::vector<unsigned char> decode(const unsigned char* compressed_data, std::size_t size);
private:
    unsigned char read_byte();

    const unsigned char* readptr;
    const unsigned char* readptr_end;
};

}

#endif
