// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_GBA_HPP
#define SHRINKLERGBACORE_GBA_HPP

#include <cstddef>

namespace shrinklergbacore
{

// Offsets in GBA cartridge header
constexpr size_t gba_header_size = 192;
constexpr size_t ofs_game_title = 0xa0;
constexpr size_t ofs_fixed_byte = 0xb2;
constexpr size_t ofs_device_type = 0xb4;
constexpr size_t ofs_game_version = 0xbc;
constexpr size_t ofs_complement = 0xbd;
constexpr size_t ofs_reserved2 = 0xbe;

}

#endif
