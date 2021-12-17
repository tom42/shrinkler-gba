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

#ifndef LIBSHRINKLER_LIBSHRINKLER_HPP
#define LIBSHRINKLER_LIBSHRINKLER_HPP

#include <vector>

namespace libshrinkler
{

class shrinkler_parameters
{
public:
    shrinkler_parameters(int preset = 2)
        : parity_context(true),
        iterations(1 * preset),
        length_margin(1 * preset),
        same_length(10 * preset),
        effort(100 * preset),
        skip_length(1000 * preset),
        references(100000)
    {}

    bool parity_context;
    int iterations;
    int length_margin;
    int same_length;
    int effort;
    int skip_length;
    int references;
};

class shrinkler
{
public:
    std::vector<unsigned char> compress(const std::vector<unsigned char>& data) const;
    void set_parameters(const shrinkler_parameters& p) { parameters = p; }
private:
    shrinkler_parameters parameters;
};

}

#endif
