// MIT License
//
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
// Copyright (c) 2021 Thomas Mathys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "shrinkler.ipp"
#include <iostream>
#include "libshrinkler/libshrinkler.hpp"

// TODO: somehow make this configurable
#define CONSOLE std::cout

namespace libshrinkler
{

std::vector<unsigned char> shrinkler::compress(const std::vector<unsigned char>& /*data*/) const
{
    CONSOLE << "Compressing..." << std::endl;

    RefEdgeFactory edge_factory(parameters.references);

    // TODO: real implementation
    return std::vector<unsigned char>();

    // TODO: port stuff below
	/* From old shrinkler::compress

    auto pack_params = create_pack_params(m_parameters);

    // For the time being we do not allow progress updates using ANSI escape sequences.
    // Problem is that in the past the Windows console did not support ANSI escape sequences at all.
    // On more recent versions of Windows it does, but this needs to be probed for and enabled:
    // https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences.
    // Not worth the trouble for the time being.
    auto packed_bytes = crunch(data, pack_params, edge_factory, false);

    CONSOLE_VERBOSE(m_console) << format("References considered: {}", edge_factory.max_edge_count) << std::endl;
    CONSOLE_VERBOSE(m_console) << format("References discarded: {}", edge_factory.max_cleaned_edges) << std::endl;
    CONSOLE_VERBOSE(m_console) << format("Uncompressed data size: {} bytes", data.size()) << std::endl;
    CONSOLE_VERBOSE(m_console) << format("Final compressed data size: {} bytes", packed_bytes.size()) << std::endl;

    if (edge_factory.max_edge_count > m_parameters.references)
    {
        CONSOLE_OUT(m_console) << "Note: compression may benefit from a larger reference buffer (-r option)" << std::endl;
    }

    return packed_bytes;
    */
    /* From Shrinkler.cpp, main()
		// Data file compression
		printf("Loading file %s...\n\n", infile);
		DataFile *orig = new DataFile;
		orig->load(infile);

		printf("Crunching...\n\n");
		RefEdgeFactory edge_factory(references.value);
		DataFile *crunched = orig->crunch(&params, &edge_factory, !no_progress.seen);
		delete orig;
		printf("References considered:%8d\n",  edge_factory.max_edge_count);
		printf("References discarded:%9d\n\n", edge_factory.max_cleaned_edges);

		printf("Saving file %s...\n\n", outfile);
		crunched->save(outfile);

		printf("Final file size: %d\n\n", crunched->size());
		delete crunched;

		if (edge_factory.max_edge_count > references.value) {
			printf("Note: compression may benefit from a larger reference buffer (-r option).\n\n");
		}

		return 0;
    */
}

}
