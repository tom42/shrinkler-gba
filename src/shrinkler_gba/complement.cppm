// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstddef>

export module shrinkler_gba:complement;
import :gba;

namespace shrinkler_gba
{

// The size of the checksummed area in the GBA cartridge header in bytes, including the complement byte.
inline constexpr size_t checksum_area_size = ofs_complement - ofs_game_title + 1;

// Calculates the complement of a GBA cartridge header.
// checksum_area must point to a buffer representing the cartridge's checksum area.
// This buffer must be at least checksum_area_size bytes big.
unsigned char calculate_complement(const unsigned char* checksum_area);

// Calculates the complement of a GBA cartridge header, but allows to choose which of the bytes
// in the checksum area should function as the complement. This allows to choose a value for
// the real complement byte and calculate the value another byte should have to get the chosen complement.
// Obviously this has no practical purpose, except for extreme size optimization.
// checksum_area must point to a buffer representing the cartridge's checksum area.
// This buffer must be at least checksum_area_size bytes big.
unsigned char calculate_complement(const unsigned char* checksum_area, size_t variable_byte_index);

}
