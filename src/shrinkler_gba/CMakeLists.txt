# SPDX-FileCopyrightText: 2025 Thomas Mathys
# SPDX-License-Identifier: MIT

set(
  shrinkler_gba_sources
  PUBLIC FILE_SET CXX_MODULES FILES
  shrinkler_gba.cppm)

# Production version of shrinkler_gba.
add_library(shrinkler_gba)
target_sources(shrinkler_gba ${shrinkler_gba_sources})
vtg_target_enable_warnings(shrinkler_gba)
