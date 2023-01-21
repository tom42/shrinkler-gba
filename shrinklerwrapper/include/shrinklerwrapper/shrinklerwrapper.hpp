// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERWRAPPER_SHRINKLERWRAPPER_HPP
#define SHRINKLERWRAPPER_SHRINKLERWRAPPER_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

namespace shrinklerwrapper
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
    shrinkler_parameters parameters;
};

}

#endif
