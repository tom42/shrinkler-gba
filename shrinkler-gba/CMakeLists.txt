# SPDX-FileCopyrightText: 2021 Thomas Mathys
# SPDX-License-Identifier: MIT
# shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

add_executable(shrinkler-gba src/main.cpp)
install(TARGETS shrinkler-gba DESTINATION bin)
target_link_libraries(shrinkler-gba PRIVATE shrinklergbacore)
