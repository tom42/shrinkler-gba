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
#include <cstdint>
#include <iostream>
#include "fmt/core.h"
#include "libshrinkler/libshrinkler.hpp"

// TODO: review entire mess in this file (parameters, whatnot, compare with latest shrinkler source...)
// TODO: somehow make this configurable
#define CONSOLE std::cout

namespace libshrinkler
{

using fmt::format;
using std::endl;
using std::runtime_error;
using std::vector;

static PackParams create_pack_params(const shrinkler_parameters& parameters)
{
    return
    {
        // TODO: make parity context selectable at runtime
        .parity_context = true,
        .iterations = parameters.iterations,
        .length_margin = parameters.length_margin,
        .skip_length = parameters.skip_length,
        .match_patience = parameters.effort,
        .max_same_length = parameters.same_length
    };
}

// Corresponds to DataFile::compress in Shrinkler.
static vector<uint32_t> compress(vector<unsigned char>& data, PackParams& params, RefEdgeFactory& edge_factory, bool show_progress)
{
    vector<uint32_t> pack_buffer;
    RangeCoder range_coder(LZEncoder::NUM_CONTEXTS + NUM_RELOC_CONTEXTS, pack_buffer);

    // Print compression status header
    // TODO: get rid of this.
    const char* ordinals[] = { "st", "nd", "rd", "th" };
    printf("Original");
    for (int p = 1; p <= params.iterations; p++) {
        printf("  After %d%s pass", p, ordinals[min(p, 4) - 1]);
    }
    printf("\n");

    // Crunch the data
    range_coder.reset();
    // TODO: nicer cast (static_cast, or maybe numeric cast...)
    // TODO: remember why we duplicated packData too. Or Fix it. What IS it printing here?
    packData(&data[0], (int)data.size(), 0, &params, &range_coder, &edge_factory, show_progress);
    range_coder.finish();

    // TODO: get rid of this
    printf("\n\n");
    fflush(stdout);

    return pack_buffer;
}

// Corresponds to DataFile::verify in Shrinkler.
int verify(vector<unsigned char>& data, vector<uint32_t>& pack_buffer, PackParams& params)
{
    CONSOLE << "Verifying..." << endl;

    RangeDecoder decoder(LZEncoder::NUM_CONTEXTS + NUM_RELOC_CONTEXTS, pack_buffer);
    LZDecoder lzd(&decoder, params.parity_context);

    // Verify data
    LZVerifier verifier(0, &data[0], (int)data.size(), (int)data.size()); // TODO: different cast (static_cast, numeric cast, whatnot)
    decoder.reset();
    decoder.setListener(&verifier);
    if (!lzd.decode(verifier))
    {
        throw runtime_error("INTERNAL ERROR: could not verify decompressed data");
    }

    // Check length
    if (verifier.size() != data.size()) // TODO: different cast (numeric)
    {
        throw runtime_error(format("INTERNAL ERROR: decompressed data has incorrect length ({}, should have been {})", verifier.size(), data.size()));
    }

    // TODO: use numeric_cast here
    // TODO: is the number printed here correct? (maybe test with lost marbles)
    //       And it seems to be possible that this becomes negative for very small input?
    //       => Well we can somehow return this as a result and have unit tests check it, no?
    return (int)(verifier.front_overlap_margin + pack_buffer.size() * 4 - data.size());
}

// Corresponds to DataFile::crunch in Shrinkler.
static vector<unsigned char> crunch(const vector<unsigned char>& data, PackParams& params, RefEdgeFactory& edge_factory, bool show_progress)
{
    // Shrinkler code uses non-const buffers all over the place. Let's create a copy then.
    vector<unsigned char> non_const_data = data;

    // Compress and verify
    vector<uint32_t> pack_buffer = compress(non_const_data, params, edge_factory, show_progress);
    auto margin = verify(non_const_data, pack_buffer, params);
    CONSOLE << "Minimum safety margin for overlapped decrunching: " << margin << endl;

    // Convert to array of bytes
    // TODO: own method and give it a name that says what it does (convert to little endian)
    vector<unsigned char> packed_bytes;
    packed_bytes.reserve(pack_buffer.size() * sizeof(pack_buffer[0]));
    for (auto word : pack_buffer)
    {
        packed_bytes.push_back(word & 0xff);
        packed_bytes.push_back((word >> 8) & 0xff);
        packed_bytes.push_back((word >> 16) & 0xff);
        packed_bytes.push_back((word >> 24) & 0xff);
    }

    return packed_bytes;
}

// Corresponds to main in Shrinkler.
vector<unsigned char> shrinkler::compress(const vector<unsigned char>& data) const
{
    CONSOLE << "Compressing..." << endl;

    RefEdgeFactory edge_factory(parameters.references);
    auto pack_params = create_pack_params(parameters);

    // For the time being we do not allow progress updates using ANSI escape sequences.
    // Problem is that in the past the Windows console did not support ANSI escape sequences at all.
    // On more recent versions of Windows it does, but this needs to be probed for and enabled:
    // https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences.
    // Not worth the trouble for the time being.
    auto packed_bytes = crunch(data, pack_params, edge_factory, false);

    CONSOLE << format("References considered: {}", edge_factory.max_edge_count) << endl;
    CONSOLE << format("References discarded: {}", edge_factory.max_cleaned_edges) << endl;
    CONSOLE << format("Uncompressed data size: {} bytes", data.size()) << endl;
    CONSOLE << format("Compressed data size: {} bytes", packed_bytes.size()) << endl;

    if (edge_factory.max_edge_count > parameters.references)
    {
        CONSOLE << "Note: compression may benefit from a larger reference buffer (-r option)" << endl;
    }

    return packed_bytes;
}

}
