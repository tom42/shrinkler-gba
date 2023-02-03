// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_COMPLEMENT_HPP
#define SHRINKLERGBACORE_COMPLEMENT_HPP

#include <cstddef>
#include "gba.hpp"

namespace shrinklergbacore
{

// The size of the complement area in the GBA cartridge header in bytes, including the complement byte.
constexpr size_t complement_area_size = ofs_complement - ofs_game_title + 1;

// TODO: document: pointer is to game title, obviously, must point at 30 bytes buffer, otherwise things will go pear-shaped
// TODO: create generalized version where the variable byte can be freely chosen
unsigned char calculate_complement(const unsigned char* game_title);

}

#endif
