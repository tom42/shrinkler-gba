// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_ADLER32_HPP
#define SHRINKLERGBACORE_ADLER32_HPP

#include <cstdint>
#include <vector>

namespace shrinklergbacore
{

uint32_t adler32(const std::vector<unsigned char>& data);

}

#endif
