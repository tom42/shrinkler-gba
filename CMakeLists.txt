# SPDX-FileCopyrightText: 2021 Thomas Mathys
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.31.6)
project(shrinkler-gba VERSION 0.1.0 LANGUAGES CXX)
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}/cmake")


################################################################################
# Options
################################################################################

option(shrinkler_gba_BUILD_TESTING "Build shrinkler-gba tests" OFF)
option(shrinkler_gba_ENABLE_WARNINGS "Build shrinkler-gba with warnings enabled" OFF)
option(shrinkler_gba_INSTALL_SHRINKLER_GBA "Install the shrinkler-gba executable" ${PROJECT_IS_TOP_LEVEL})


################################################################################
# Set up testing
################################################################################

if(shrinkler_gba_BUILD_TESTING)
  enable_testing()
  include(VtgTesting)
  vtg_testing_setup_catch2(3.4.0)
endif()


################################################################################
# Get dependencies
################################################################################

include(FetchContent)

FetchContent_Declare(
  agbpack
  GIT_REPOSITORY    https://github.com/tom42/agbpack
  GIT_TAG           v0.1.0
  SYSTEM
  FIND_PACKAGE_ARGS 0.1.0 NAMES agbpack)
FetchContent_MakeAvailable(agbpack)

FetchContent_Declare(
  argpppp
  GIT_REPOSITORY    https://github.com/tom42/argpppp
  GIT_TAG           v0.2.0
  SYSTEM
  FIND_PACKAGE_ARGS 0.2.0 NAMES argpppp)
FetchContent_MakeAvailable(argpppp)

FetchContent_Declare(
  elfio
  GIT_REPOSITORY    https://github.com/serge1/ELFIO
  GIT_TAG           Release_3.12
  SYSTEM
  FIND_PACKAGE_ARGS 3.12 NAMES elfio)
FetchContent_MakeAvailable(elfio)

# Note: the ms-gsl 4.2.0 package available in vcpkg contains the version 4.1.0
# in its CMake version file, so this will always fetch sources and never use
# the installed package. We can live with this for the time being.
FetchContent_Declare(
  gsl
  GIT_REPOSITORY    https://github.com/microsoft/GSL
  GIT_TAG           v4.2.0
  SYSTEM
  FIND_PACKAGE_ARGS 4.2.0 NAMES Microsoft.GSL)
FetchContent_MakeAvailable(gsl)

FetchContent_Declare(
  libshrinkler
  GIT_REPOSITORY    https://github.com/tom42/libshrinkler
  GIT_TAG           v1.1.0
  SYSTEM
  FIND_PACKAGE_ARGS 1.1.0 NAMES libshrinkler)
FetchContent_MakeAvailable(libshrinkler)

FetchContent_Declare(
  lzasm
  GIT_REPOSITORY    https://github.com/tom42/lzasm
  GIT_TAG           v0.5.0
  SYSTEM
  FIND_PACKAGE_ARGS 0.5.0 NAMES lzasm)
FetchContent_MakeAvailable(lzasm)


################################################################################
# Build our own code
################################################################################

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(shrinkler_gba_ENABLE_WARNINGS)
  set(vtg_ENABLE_WARNINGS ON)
endif()
include(VtgEnableWarnings)

add_subdirectory(src)
if(shrinkler_gba_BUILD_TESTING)
  add_subdirectory(test)
endif()


################################################################################
# Add custom targets (reuse lint etc.)
################################################################################

if(PROJECT_IS_TOP_LEVEL)
  add_custom_target(
    reuse-lint
    COMMAND reuse lint
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
endif()
