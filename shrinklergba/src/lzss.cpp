// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <stdexcept>
#include "shrinklergba/gba_bios_common.hpp"
#include "shrinklergba/lzss.hpp"

namespace shrinklergba
{

constexpr std::size_t ofs_compressed_data_stream = 4;

std::vector<unsigned char> lzss_decoder::decode(const std::vector<unsigned char>& compressed_data)
{
    return decode(compressed_data.data(), compressed_data.size());
}

std::vector<unsigned char> lzss_decoder::decode(const unsigned char* compressed_data, std::size_t size)
{
    throw_if_wrong_compressed_size(size);
    throw_if_wrong_compression_type(compression_type::lzss, compressed_data[ofs_compression_type] >> 4);
    const std::size_t decompressed_size = get_decompressed_size(compressed_data);
    
    std::vector<unsigned char> decompressed_data;
    decompressed_data.reserve(decompressed_size);

    unsigned char tagbits = 0;
    unsigned char bitmask = 0;
    readptr = compressed_data + ofs_compressed_data_stream;
    readptr_end = compressed_data + size;

    while (decompressed_data.size() < decompressed_size)
    {
        bitmask >>= 1;
        if (!bitmask)
        {
            tagbits = read_byte();
            bitmask = 0x80;
        }

        if (tagbits & bitmask)
        {
            unsigned char b0 = read_byte();
            unsigned char b1 = read_byte();
            std::size_t length = ((b0 >> 4) & 15) + 3;
            std::size_t offset = ((b0 & 15) << 8) | b1;

            if (decompressed_data.size() + length > decompressed_size)
            {
                throw std::runtime_error("wrong match length");
            }

            while (length--)
            {
                decompressed_data.push_back(decompressed_data[decompressed_data.size() - offset - 1]);
            }
        }
        else
        {
            decompressed_data.push_back(read_byte());
        }
    }

    return decompressed_data;
}

unsigned char lzss_decoder::read_byte()
{
    if (readptr >= readptr_end)
    {
        throw std::runtime_error("unexpected end of compressed data");
    }

    return *readptr++;
}

}
