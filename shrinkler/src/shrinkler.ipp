// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

// shrinkler.ipp: include all of Shrinkler's code we need.
// Before doing so, disable selected warnings. Afterwards, enable them again.

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wnarrowing" // Only required for 32 bit targets
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4018 4100 4244 4389 4456 4458)
#if defined(_WIN64)
#pragma warning(disable: 4267)
#endif
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../../3rdparty/Shrinkler/cruncher/HunkFile.h"
#include "../../3rdparty/Shrinkler/cruncher/Pack.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#undef _CRT_SECURE_NO_WARNINGS
#endif
