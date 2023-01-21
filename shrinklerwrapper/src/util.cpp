// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include "util.hpp"

namespace shrinklerwrapper::detail
{

std::vector<unsigned char> to_little_endian(const std::vector<uint32_t>& buffer)
{
    std::vector<unsigned char> packed_bytes;
    packed_bytes.reserve(buffer.size() * sizeof(buffer[0]));

    for (auto word : buffer)
    {
        packed_bytes.push_back(word & 0xff);
        packed_bytes.push_back((word >> 8) & 0xff);
        packed_bytes.push_back((word >> 16) & 0xff);
        packed_bytes.push_back((word >> 24) & 0xff);
    }

    return packed_bytes;
}

}
