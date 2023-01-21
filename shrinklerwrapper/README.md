<!--
SPDX-FileCopyrightText: 2021 Thomas Mathys
SPDX-License-Identifier: MIT
shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
-->

This directory contains a library that wraps the original Shrinkler code and exposes
its interesting functionality through a simple facade class called shrinkler_compressor.

We do this because we like our code being compiled with warnings being treated as
errors and with warning levels much higher than what's possible with Shrinkler's code.

Now much of Shrinkler's code consists of header files, so we can't solve this by having
the Shrinkler code in the same library as our own code and using different warning flags
on a per file basis.

So we wrap Shrinkler into a simple library, which we compile with relatively low warning
levels (see shrinkler.ipp). This library's public header is written such that it does
not pull in any Shrinkler code and thus does not cause warnings in our own code.
