// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#if defined(__CYGWIN__)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#define _POSIX_C_SOURCE 200112L
#endif

#define BOOST_TEST_MODULE shrinklergba-unittest
#include <boost/test/included/unit_test.hpp>
