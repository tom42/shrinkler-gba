// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <vector>
#include "shrinklerwrapper/shrinklerwrapper.hpp"

class Coder;
struct PackParams;
class RefEdgeFactory;

namespace shrinklerwrapper::detail
{

class shrinkler_compressor_impl final
{
public:
    shrinkler_compressor_impl(const shrinkler_parameters& parameters) : parameters(parameters) {}

    std::vector<unsigned char> compress(const std::vector<unsigned char>& data) const;
private:
    std::vector<unsigned char> crunch(const std::vector<unsigned char>& data, PackParams& params, RefEdgeFactory& edge_factory, bool show_progress) const;
    std::vector<uint32_t> compress(std::vector<unsigned char>& data, PackParams& params, RefEdgeFactory& edge_factory, bool show_progress) const;

    static_assert(sizeof(ptrdiff_t) >= sizeof(size_t));
    ptrdiff_t verify(std::vector<unsigned char>& data, std::vector<uint32_t>& pack_buffer, PackParams& params) const;

    void packData(unsigned char* data, int data_length, int zero_padding, PackParams* params, Coder* result_coder, RefEdgeFactory* edge_factory, bool show_progress) const;

    shrinkler_parameters parameters;
};

}
