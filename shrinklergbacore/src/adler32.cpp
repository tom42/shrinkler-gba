// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include "shrinklergbacore/adler32.hpp"

namespace shrinklergbacore
{

uint32_t adler32(const std::vector<unsigned char>& data)
{
    constexpr auto BASE = 65521;

    uint32_t s1 = 1;
    uint32_t s2 = 0;

    for (auto byte : data)
    {
        s1 = (s1 + byte) % BASE;
        s2 = (s2 + s1) % BASE;
    }

    return (s2 << 16) | s1;
}

}
