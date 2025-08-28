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

# TODO: internally, call everything shrinklergba: no dashes, no underscores
# TODO: rewrite similarly to agbpack etc.
# TODO: merge in stuff from old CMakeLists as it makes sense
# TODO: implement all options, including installer

################################################################################
# Set up testing
################################################################################

if(shrinkler_gba_BUILD_TESTING)
  enable_testing()
  include(VtgTesting)
  vtg_testing_setup_catch2(3.4.0)
endif()


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
if(agbpack_BUILD_TESTING)
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
