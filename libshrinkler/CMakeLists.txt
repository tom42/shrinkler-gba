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
  src/shrinkler.cpp)

add_library(libshrinkler ${SOURCES})
target_include_directories(
  libshrinkler
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE
  # Add Shrinkler's decruncher directory to the list of include directories,
  # but do not add 3rdparty/shrinkler/cruncher which contains Shrinkler's
  # C++ sources. This is because Shrinkler relies on being able to include
  # its own assert.h along with the cassert and assert.h system headers.
  # If the directory containing Shrinkler's assert.h is in the search path,
  # then that will not work. Not with MSVC anyway.
  "${PROJECT_SOURCE_DIR}/3rdparty/shrinkler/decrunchers_bin")
target_link_libraries(libshrinkler PRIVATE fmt)

if(BUILD_TESTING AND Boost_FOUND)
  add_executable(
    libshrinkler-unittest
    unittest/main.cpp
    unittest/shrinkler_parameters_test.cpp
    unittest/shrinkler_test.cpp)
  target_include_directories(libshrinkler-unittest PRIVATE "${Boost_INCLUDE_DIRS}")
  target_link_libraries(libshrinkler-unittest PRIVATE libshrinkler)
  add_test(NAME libshrinkler-unittest COMMAND libshrinkler-unittest)
endif()
