// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include "shrinklergbacore/depacker.hpp"

namespace shrinklergbacore
{

depacker::depacker()
{
    assemble();
}

void depacker::assemble()
{
    // TODO: now assemble that cart already!
    //       * So what do we need?
    //         * The compressed intro, somehow
    //         * The load address
    //         * The entry point
    //         * Any sort of options (which we do not have yet, but...oh well)
}

}
