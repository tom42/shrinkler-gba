# SPDX-FileCopyrightText: 2021 Thomas Mathys
# SPDX-License-Identifier: MIT
# shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

configure_file(shrinklergbacore_version.hpp.in shrinklergbacore_version.hpp)

set(
  SOURCES
  include/shrinklergbacore/adler32.hpp
  include/shrinklergbacore/cart_assembler.hpp
  include/shrinklergbacore/command_line.hpp
  include/shrinklergbacore/complement.hpp
  include/shrinklergbacore/console.hpp
  include/shrinklergbacore/elfio_wrapper.hpp
  include/shrinklergbacore/elf_strings.hpp
  include/shrinklergbacore/gba.hpp
  include/shrinklergbacore/gba_packer.hpp
  include/shrinklergbacore/input_file.hpp
  include/shrinklergbacore/options.hpp
  include/shrinklergbacore/table_printer.hpp
  src/adler32.cpp
  src/cart_assembler.cpp
  src/command_line.cpp
  src/complement.cpp
  src/elf_strings.cpp
  src/gba_packer.cpp
  src/input_file.cpp
  src/table_printer.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_library(shrinklergbacore ${SOURCES})
target_include_directories(
  shrinklergbacore
  PUBLIC
  include
  PRIVATE
  "${Boost_INCLUDE_DIRS}"
  "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(shrinklergbacore PUBLIC shrinklerwrapper PRIVATE elfio fmt lzasm)
if(NOT HAVE_ARGP)
  target_link_libraries(shrinklergbacore PRIVATE argp-standalone)
endif()

if(BUILD_TESTING)
  set(SHRINKLERGBACORE_UNITTEST_TESTDATA_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/testdata")
  configure_file(shrinklergbacore_unittest_config.hpp.in shrinklergbacore_unittest_config.hpp)

  set(
    UNITTEST_SOURCES
    unittest/adler32_test.cpp
    unittest/command_line_test.cpp
    unittest/complement_test.cpp
    unittest/input_file_test.cpp
    unittest/main.cpp
    unittest/options_test.cpp
    unittest/test_utilities.cpp
    unittest/test_utilities.hpp)

  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${UNITTEST_SOURCES})

  add_executable(shrinklergbacore-unittest ${UNITTEST_SOURCES})
  target_include_directories(
    shrinklergbacore-unittest
    PRIVATE
    "${Boost_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_BINARY_DIR}")
  target_link_libraries(shrinklergbacore-unittest PRIVATE shrinklergbacore)
  add_test(NAME shrinklergbacore-unittest COMMAND shrinklergbacore-unittest)
endif()
