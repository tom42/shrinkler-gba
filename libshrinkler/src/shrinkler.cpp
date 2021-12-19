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

#include <boost/numeric/conversion/cast.hpp>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include "fmt/core.h"
#include "libshrinkler/libshrinkler.hpp"

// TODO: review entire mess in this file (parameters, whatnot, compare with latest shrinkler source...)
// TODO: somehow make this configurable
#define CONSOLE_OLD std::cout
#define CONSOLE if (!parameters.verbose); else std::cout

namespace libshrinkler
{

using boost::numeric_cast;
using fmt::format;
using std::endl;
using std::runtime_error;
using std::vector;

static PackParams create_pack_params(const shrinkler_parameters& parameters)
{
    return
    {
        .parity_context = parameters.parity_context,
        .iterations = parameters.iterations,
        .length_margin = parameters.length_margin,
        .skip_length = parameters.skip_length,
        .match_patience = parameters.effort,
        .max_same_length = parameters.same_length
    };
}

static void packData2(unsigned char* data, int data_length, int zero_padding, PackParams* params, Coder* result_coder, RefEdgeFactory* edge_factory, bool show_progress)
{
    MatchFinder finder(data, data_length, 2, params->match_patience, params->max_same_length);
    LZParser parser(data, data_length, zero_padding, finder, params->length_margin, params->skip_length, edge_factory);
    result_size_t real_size = 0;
    result_size_t best_size = (result_size_t)1 << (32 + 3 + Coder::BIT_PRECISION);
    int best_result = 0;
    vector<LZParseResult> results(2);
    CountingCoder* counting_coder = new CountingCoder(LZEncoder::NUM_CONTEXTS);
    LZProgress* progress;
    if (show_progress) {
        progress = new PackProgress();
    }
    else {
        progress = new NoProgress();
    }
    CONSOLE_OLD << "Original: " << data_length << endl;
    for (int i = 0; i < params->iterations; i++) {
        // Parse data into LZ symbols
        LZParseResult& result = results[1 - best_result];
        Coder* measurer = new SizeMeasuringCoder(counting_coder);
        measurer->setNumberContexts(LZEncoder::NUMBER_CONTEXT_OFFSET, LZEncoder::NUM_NUMBER_CONTEXTS, data_length);
        finder.reset();
        result = parser.parse(LZEncoder(measurer, params->parity_context), progress);
        delete measurer;

        // Encode result using adaptive range coding
        vector<unsigned> dummy_result;
        RangeCoder* range_coder = new RangeCoder(LZEncoder::NUM_CONTEXTS, dummy_result);
        real_size = result.encode(LZEncoder(range_coder, params->parity_context));
        range_coder->finish();
        delete range_coder;

        // Choose if best
        if (real_size < best_size) {
            best_result = 1 - best_result;
            best_size = real_size;
        }

        // Print size
        CONSOLE_OLD << format("Pass {}: {:.3f}", i + 1, real_size / (double)(8 << Coder::BIT_PRECISION)) << endl;

        // Count symbol frequencies
        CountingCoder* new_counting_coder = new CountingCoder(LZEncoder::NUM_CONTEXTS);
        result.encode(LZEncoder(counting_coder, params->parity_context));

        // New size measurer based on frequencies
        CountingCoder* old_counting_coder = counting_coder;
        counting_coder = new CountingCoder(old_counting_coder, new_counting_coder);
        delete old_counting_coder;
        delete new_counting_coder;
    }
    delete progress;
    delete counting_coder;

    results[best_result].encode(LZEncoder(result_coder, params->parity_context));
}

// Corresponds to DataFile::compress in Shrinkler.
static vector<uint32_t> compress(vector<unsigned char>& data, PackParams& params, RefEdgeFactory& edge_factory, bool show_progress)
{
    vector<uint32_t> pack_buffer;
    RangeCoder range_coder(LZEncoder::NUM_CONTEXTS + NUM_RELOC_CONTEXTS, pack_buffer);

    // Crunch the data
    range_coder.reset();
    packData2(&data[0], numeric_cast<int>(data.size()), 0, &params, &range_coder, &edge_factory, show_progress);
    range_coder.finish();

    return pack_buffer;
}

template <typename T>
std::make_signed_t<T> signed_cast(T value)
{
    return value;
}

// Corresponds to DataFile::verify in Shrinkler.
auto verify(vector<unsigned char>& data, vector<uint32_t>& pack_buffer, PackParams& params)
{
    CONSOLE_OLD << "Verifying..." << endl;

    RangeDecoder decoder(LZEncoder::NUM_CONTEXTS + NUM_RELOC_CONTEXTS, pack_buffer);
    LZDecoder lzd(&decoder, params.parity_context);

    // Verify data
    LZVerifier verifier(0, &data[0], numeric_cast<int>(data.size()), numeric_cast<int>(data.size()));
    decoder.reset();
    decoder.setListener(&verifier);
    if (!lzd.decode(verifier))
    {
        throw runtime_error("INTERNAL ERROR: could not verify decompressed data");
    }

    // Check length
    if (numeric_cast<size_t>(verifier.size()) != data.size())
    {
        throw runtime_error(format("INTERNAL ERROR: decompressed data has incorrect length ({}, should have been {})", verifier.size(), data.size()));
    }

    return signed_cast(verifier.front_overlap_margin + pack_buffer.size() * 4 - data.size());
}

static vector<unsigned char> to_little_endian(const vector<uint32_t>& pack_buffer)
{
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

// Corresponds to DataFile::crunch in Shrinkler.
vector<unsigned char> shrinkler::crunch(const vector<unsigned char>& data, PackParams& params, RefEdgeFactory& edge_factory, bool show_progress) const
{
    // Shrinkler code uses non-const buffers all over the place, so we create a copy of the original data.
    vector<unsigned char> non_const_data = data;

    // Compress and verify
    vector<uint32_t> pack_buffer = libshrinkler::compress(non_const_data, params, edge_factory, show_progress); // TODO: remove full qualification
    auto margin = verify(non_const_data, pack_buffer, params);
    CONSOLE << "Minimum safety margin for overlapped decrunching: " << margin << endl;

    // Shrinkler produces packed data suitable for 68k CPUs.
    // For the GBA's ARM7TDMI convert the data to little endian.
    return to_little_endian(pack_buffer);
}

}
