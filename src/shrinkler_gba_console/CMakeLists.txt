# SPDX-FileCopyrightText: 2025 Thomas Mathys
# SPDX-License-Identifier: MIT

# Executable is called shrinkler-gba, IDE projects are called shrinkler_gba_console.
add_executable(shrinkler-gba main.cpp)
set_property(TARGET shrinkler-gba PROPERTY PROJECT_LABEL "shrinkler_gba_console")

vtg_target_enable_warnings(shrinkler-gba)
target_link_libraries(shrinkler-gba shrinkler_gba)

# TODO: install
