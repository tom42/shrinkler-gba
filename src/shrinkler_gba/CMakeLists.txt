# SPDX-FileCopyrightText: 2025 Thomas Mathys
# SPDX-License-Identifier: MIT

set(
  shrinkler_gba_sources
  PUBLIC FILE_SET CXX_MODULES FILES
  adler32.cppm
  shrinkler_gba.cppm
  PRIVATE
  adler32.cpp)

# Production version of shrinkler_gba.
add_library(shrinkler_gba)
target_sources(shrinkler_gba ${shrinkler_gba_sources})
target_compile_definitions(shrinkler_gba PRIVATE SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING=)
vtg_target_enable_warnings(shrinkler_gba)

# Unit testing version of shrinkler_gba.
if(shrinkler_gba_BUILD_TESTING)
  add_library(shrinkler_gba_unit_testing)
  target_sources(shrinkler_gba_unit_testing ${shrinkler_gba_sources})
  target_compile_definitions(shrinkler_gba_unit_testing PRIVATE SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING=export)
  vtg_target_enable_warnings(shrinkler_gba_unit_testing)
endif()
