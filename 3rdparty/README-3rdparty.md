<!--
SPDX-FileCopyrightText: 2021 Thomas Mathys
SPDX-License-Identifier: MIT
shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
-->

# 3rd party code

This directory contains 3rd party libraries as Git subtrees.
For general information on Git subtree refer to https://www.atlassian.com/git/tutorials/git-subtree.

## How to add a new library
Examples:
* `git subtree add --prefix 3rdparty/Shrinkler https://github.com/askeksa/Shrinkler master --squash`
* `git subtree add --prefix 3rdparty/argp-standalone https://github.com/tom42/argp-standalone master --squash`
* `git subtree add --prefix 3rdparty/ELFIO https://github.com/serge1/ELFIO Release_3.9 --squash`
* `git subtree add --prefix 3rdparty/fmt https://github.com/fmtlib/fmt 8.0.1 --squash`
* `git subtree add --prefix 3rdparty/lzasm https://github.com/tom42/lzasm v0.1.0 --squash`

Note:
* The commands need to be issued from the top level source directory.
* Do not have `./` in front of `3rdparty`

## How to update an existing library
Example: `git subtree pull --prefix 3rdparty/lzasm https://github.com/tom42/lzasm v0.2.0 --squash`
