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

# TODO: rewrite similarly to agbpack etc.
# TODO: merge in stuff from old CMakeLists as it makes sense
# TODO: implement all options, including installer

################################################################################
# Set up testing
################################################################################

# TODO: enable testing (through preset, then that's already done) and get it to work (VtgTesting is probably missing)
# TODO: figure out which version of catch2 we can easily use on github, and use that
if(shrinkler_gba_BUILD_TESTING)
  enable_testing()
  include(VtgTesting)
  vtg_testing_setup_catch2(3.4.0)
endif()


################################################################################
# Build our own code
################################################################################
