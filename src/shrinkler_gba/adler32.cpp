// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT

module shrinkler_gba;

namespace shrinkler_gba
{

// TODO: are we not missing headers here, like <cstdint> and <vector>? That's just MSVC not needing them, no?
uint32_t adler32(const std::vector<unsigned char>& data)
{
    constexpr auto base = 65521;

    uint32_t s1 = 1;
    uint32_t s2 = 0;

    for (auto byte : data)
    {
        s1 = (s1 + byte) % base;
        s2 = (s2 + s1) % base;
    }

    return (s2 << 16) | s1;
}

}
