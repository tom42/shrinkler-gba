// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLER_SHRINKLER_HPP
#define SHRINKLER_SHRINKLER_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

// TODO: this is ugly. Keep these out of this header. If this means creating
//       another class that is used internally by shrinkler_compressor,
//       then be it so
class Coder;
struct PackParams;
class RefEdgeFactory;

namespace shrinkler
{

class shrinkler_parameters final
{
public:
    explicit shrinkler_parameters(int preset = 2)
    {
        this->preset(preset);
    }

    void preset(int preset)
    {
        iterations = 1 * preset;
        length_margin = 1 * preset;
        same_length = 10 * preset;
        effort = 100 * preset;
        skip_length = 1000 * preset;
    }

    bool verbose = false;
    bool parity_context = true;
    int references = 100000;
    int iterations;
    int length_margin;
    int same_length;
    int effort;
    int skip_length;
};

class shrinkler_compressor final
{
public:
    std::vector<unsigned char> compress(const std::vector<unsigned char>& data) const;
    void set_parameters(const shrinkler_parameters& p) { parameters = p; }
private:
    std::vector<unsigned char> crunch(const std::vector<unsigned char>& data, PackParams& params, RefEdgeFactory& edge_factory, bool show_progress) const;
    std::vector<uint32_t> compress(std::vector<unsigned char>& data, PackParams& params, RefEdgeFactory& edge_factory, bool show_progress) const;

    static_assert(sizeof(ptrdiff_t) >= sizeof(size_t));
    ptrdiff_t verify(std::vector<unsigned char>& data, std::vector<uint32_t>& pack_buffer, PackParams& params) const;

    void packData(unsigned char* data, int data_length, int zero_padding, PackParams* params, Coder* result_coder, RefEdgeFactory* edge_factory, bool show_progress) const;

    shrinkler_parameters parameters;
};

}

#endif
