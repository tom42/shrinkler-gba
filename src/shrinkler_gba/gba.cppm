// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstddef>
#include <cstdint>

export module shrinkler_gba:gba;

namespace shrinkler_gba
{

// GBA memory areas
inline constexpr uint32_t mem_bg_palette = 0x05000000;
inline constexpr uint32_t mem_vram = 0x06000000;
inline constexpr uint32_t mem_rom = 0x08000000;

// Offsets in GBA cartridge header
inline constexpr size_t gba_header_size = 192;
SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING inline constexpr size_t ofs_game_title = 0xa0;
inline constexpr size_t ofs_fixed_byte = 0xb2;
inline constexpr size_t ofs_device_type = 0xb4;
SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING inline constexpr size_t ofs_game_version = 0xbc;
inline constexpr size_t ofs_complement = 0xbd;
inline constexpr size_t ofs_reserved2 = 0xbe;

}
