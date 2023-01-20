# SPDX-FileCopyrightText: 2021 Thomas Mathys
# SPDX-License-Identifier: MIT
# shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

if(NOT HAVE_ARGP)
  add_subdirectory(argp-standalone)
endif()

add_subdirectory(ELFIO)
add_subdirectory(fmt)
add_subdirectory(lzasm)
