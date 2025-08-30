// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstdint>
#include <vector>

export module shrinkler_gba:adler32;

namespace shrinkler_gba
{

uint32_t adler32(const std::vector<unsigned char>& data);

}
