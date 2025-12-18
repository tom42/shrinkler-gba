// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>

export module shrinkler_gba:utility;

namespace shrinkler_gba
{

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
using bytevector = std::vector<unsigned char>;

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
uint32_t adler32(const bytevector& data);

std::runtime_error internal_error(std::string_view s);

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
template <std::unsigned_integral TUnsigned>
constexpr auto to_signed(TUnsigned u)
{
    return static_cast<std::make_signed_t<TUnsigned>>(u);
}

}
