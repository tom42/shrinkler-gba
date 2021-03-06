# MIT License
#
# shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
# Copyright (c) 2021 Thomas Mathys
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

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
