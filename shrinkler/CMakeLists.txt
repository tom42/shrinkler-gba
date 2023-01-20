# SPDX-FileCopyrightText: 2021 Thomas Mathys
# SPDX-License-Identifier: MIT
# shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

set(
  SOURCES
  include/shrinkler/shrinkler.hpp
  src/shrinkler_compressor.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_library(shrinkler ${SOURCES})
target_include_directories(
  shrinkler
  PUBLIC
  include
  PRIVATE
  "${Boost_INCLUDE_DIRS}"
  # Add Shrinkler's decruncher directory to the list of include directories,
  # but do not add 3rdparty/shrinkler/cruncher which contains Shrinkler's
  # C++ sources. This is because Shrinkler relies on being able to include
  # its own assert.h along with the cassert and assert.h system headers.
  # If the directory containing Shrinkler's assert.h is in the search path,
  # then that will not work. Not with MSVC anyway.
  "${PROJECT_SOURCE_DIR}/3rdparty/Shrinkler/decrunchers_bin")
target_link_libraries(shrinkler PRIVATE fmt)

if(BUILD_TESTING)
  add_executable(
    shrinkler-unittest
    unittest/main.cpp
    unittest/shrinkler_parameters_test.cpp
    unittest/shrinkler_compressor_test.cpp)
  target_include_directories(shrinkler-unittest PRIVATE "${Boost_INCLUDE_DIRS}")
  target_link_libraries(shrinkler-unittest PRIVATE shrinkler)
  add_test(NAME shrinkler-unittest COMMAND shrinkler-unittest)
endif()
