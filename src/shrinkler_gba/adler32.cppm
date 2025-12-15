// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstdint>
#include <vector>

export module shrinkler_gba:adler32;
import :utility;

namespace shrinkler_gba
{

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
uint32_t adler32(const bytevector& data);

}
