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
# * Place VtgAddUninstallTarget.cmake and vtg_cmake_uninstall.cmake.in
#   in the same directory. A good choice might be ${PROJECT_SOURCE_DIR }/cmake.
#   That is, a cmake directory located in your CMake project's top-level directory.
# * In your top-level CMakeLists.txt, include VtgAddUninstallTarget.cmake
#   Note that a project should only do so if it is the top-level project,
#   so as not to pollute parent projects with unwanted custom targets.
#
# Example (assuming the directory containing VtgAddUninstallTarget.cmake is in CMake's include path):
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
