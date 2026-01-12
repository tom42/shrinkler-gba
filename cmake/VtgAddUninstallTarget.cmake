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

# TODO:
# * Add documentation:
#   * how to use: include in top level CMakeLists.txt, but preferably only if we're the top level project (we should not add unsolicited uninstall targets)
#   * Where to place this file: cmake directory (and along with it the other cmake script we're going to configure)
# * Implement it
#   * Well actually this is going to get the 2nd portion of that snippet from the Wiki, no?
