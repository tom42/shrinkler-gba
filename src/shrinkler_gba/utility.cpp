// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

module shrinkler_gba;

namespace shrinkler_gba
{

uint32_t adler32(const bytevector& data)
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

std::runtime_error internal_error(std::string_view s)
{
    std::string msg("INTERNAL ERROR: ");
    msg += s;
    return std::runtime_error(msg);
}

}
