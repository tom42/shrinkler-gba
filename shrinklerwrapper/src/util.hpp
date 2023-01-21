// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERWRAPPER_UTIL_HPP
#define SHRINKLERWRAPPER_UTIL_HPP

#include <cstdint>
#include <vector>

namespace shrinklerwrapper::detail
{

std::vector<unsigned char> to_little_endian(const std::vector<uint32_t>& buffer);

}

#endif
