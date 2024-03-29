# SPDX-FileCopyrightText: 2021 Thomas Mathys
# SPDX-License-Identifier: MIT
# shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

cmake_minimum_required(VERSION 3.20.0)
project(shrinkler-gba VERSION 0.1.0 LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(IS_MAIN_PROJECT TRUE)
else()
  set(IS_MAIN_PROJECT FALSE)
endif()

if(IS_MAIN_PROJECT)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  include(CTest)
endif()


################################################################################
# Find packages.
################################################################################

include(CheckFunctionExists)

# Search for Boost headers only, but not the libraries.
# We'll compile the unit test framework ourselves as part of the unit tests.
# This simplifies cross development, since we only
# need Boost headers on build machines.
find_package(Boost 1.78 REQUIRED)

check_function_exists(argp_parse HAVE_ARGP)


################################################################################
# Subdirectories with 3rd party code.
################################################################################

add_subdirectory(3rdparty)


################################################################################
# Subdirectories with own code.
################################################################################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(shrinklerwrapper)
add_subdirectory(shrinklergbacore)
add_subdirectory(shrinkler-gba)
