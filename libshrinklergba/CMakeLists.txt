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

set(
  SOURCES
  include/libshrinklergba/command_line.hpp
  include/libshrinklergba/options.hpp
  src/command_line.cpp)

add_library(libshrinklergba ${SOURCES})
target_include_directories(
  libshrinklergba
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(libshrinklergba PUBLIC libshrinkler)

if(BUILD_TESTING)
  add_executable(
    libshrinklergba-unittest
    unittest/command_line_test.cpp
    unittest/main.cpp
    unittest/options_test.cpp)
  target_include_directories(libshrinklergba-unittest PRIVATE "${Boost_INCLUDE_DIRS}")
  target_link_libraries(libshrinklergba-unittest PRIVATE libshrinklergba)
  add_test(NAME libshrinklergba-unittest COMMAND libshrinklergba-unittest)
endif()
