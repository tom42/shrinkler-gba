# SPDX-FileCopyrightText: Copyright Kitware, Inc. and Contributors
# SPDX-License-Identifier: BSD-3-Clause
#
# This scripts adds a custom target 'uninstall' that removes installed files according to install_manifest.txt.
#
# Based on: https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake.
# Regarding licensing, https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#what-is-its-license states that
#
#   "CMake is distributed under the OSI-approved BSD 3-clause License. [...]
#   The snippets on this wiki are provided under the same license."
#
# Usage:
# * In your top-level CMakeLists.txt, include VtgAddUninstallTarget.cmake
#
# Example:
#
#   if(PROJECT_IS_TOP_LEVEL)
#     include(VtgAddUninstallTarget)
#   endif()

# Add uninstall target
# Note: unlike the original code from the CMake wiki we use CMAKE_CURRENT_LIST_DIR to find the .in file.
# The idea is that vtg_cmake_uninstall.cmake.in is always placed besides VtgAddUninstallTarget.cmake,
# wherever that may be. By using CMAKE_CURRENT_LIST_DIR inside VtgAddUninstallTarget.cmake,
# VtgAddUninstallTarget.cmake will always be able to find vtg_cmake_uninstall.cmake.in.
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/vtg_cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/vtg_cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(
    uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/vtg_cmake_uninstall.cmake)
endif()

# TODO:
# * Add documentation:
#   * how to use: include in top level CMakeLists.txt, but preferably only if we're the top level project (we should not add unsolicited uninstall targets)
#   * Where to place this file: cmake directory (and along with it the other cmake script we're going to configure)
