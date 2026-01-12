# SPDX-FileCopyrightText: 2025 Thomas Mathys
# SPDX-License-Identifier: MIT
#
# This script adds a reuse-lint custom target that runs the command 'reuse lint'.
#
# Usage:
# * In your top-level CMakeLists.txt, include VtgAddReuseLintTarget.cmake
# * A project should only do so if it is the top-level project,
#   so as not to pollute parent projects with unwanted custom targets.
#
# Example:
#
#   if(PROJECT_IS_TOP_LEVEL)
#     include(VtgAddReuseLintTarget)
#   endif()

add_custom_target(
  reuse-lint
  COMMAND reuse lint
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
