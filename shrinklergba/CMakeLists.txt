# SPDX-FileCopyrightText: 2021 Thomas Mathys
# SPDX-License-Identifier: MIT
# shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

configure_file(shrinklergba_version.hpp.in shrinklergba_version.hpp)

set(
  SOURCES
  include/shrinklergba/command_line.hpp
  include/shrinklergba/console.hpp
  include/shrinklergba/elfio_wrapper.hpp
  include/shrinklergba/elf_strings.hpp
  include/shrinklergba/gba_bios_common.hpp
  include/shrinklergba/gba_packer.hpp
  include/shrinklergba/huffman.hpp
  include/shrinklergba/input_file.hpp
  include/shrinklergba/lzss.hpp
  include/shrinklergba/options.hpp
  include/shrinklergba/table_printer.hpp
  src/command_line.cpp
  src/elf_strings.cpp
  src/gba_bios_common.cpp
  src/gba_packer.cpp
  src/huffman.cpp
  src/input_file.cpp
  src/lzss.cpp
  src/table_printer.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_library(shrinklergba ${SOURCES})
target_include_directories(
  shrinklergba
  PUBLIC
  include
  PRIVATE
  "${Boost_INCLUDE_DIRS}"
  "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(shrinklergba PUBLIC shrinkler PRIVATE elfio fmt lzasm)
if(NOT HAVE_ARGP)
  target_link_libraries(shrinklergba PRIVATE argp-standalone)
endif()

if(BUILD_TESTING)
  set(SHRINKLERGBA_UNITTEST_TESTDATA_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/testdata")
  configure_file(shrinklergba_unittest_config.hpp.in shrinklergba_unittest_config.hpp)

  set(
    UNITTEST_SOURCES
    unittest/command_line_test.cpp
    unittest/huffman_decoder_test.cpp
    unittest/input_file_test.cpp
    unittest/lzss_decoder_test.cpp
    unittest/main.cpp
    unittest/options_test.cpp
    unittest/test_utilities.cpp
    unittest/test_utilities.hpp)

  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${UNITTEST_SOURCES})

  add_executable(shrinklergba-unittest ${UNITTEST_SOURCES})
  target_include_directories(
    shrinklergba-unittest
    PRIVATE
    "${Boost_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_BINARY_DIR}")
  target_link_libraries(shrinklergba-unittest PRIVATE shrinklergba)
  add_test(NAME shrinklergba-unittest COMMAND shrinklergba-unittest)
endif()
