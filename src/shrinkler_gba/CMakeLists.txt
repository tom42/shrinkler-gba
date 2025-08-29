# SPDX-FileCopyrightText: 2025 Thomas Mathys
# SPDX-License-Identifier: MIT

set(
  shrinkler_gba_sources
  PUBLIC FILE_SET CXX_MODULES FILES
  shrinkler_gba.cppm)

add_library(shrinkler_gba)
target_sources(shrinkler_gba ${shrinkler_gba_sources})
# TODO: enable warnings
# TODO: see what else we need for a module, e.g. from agbpack (do we need to set the standard?)
