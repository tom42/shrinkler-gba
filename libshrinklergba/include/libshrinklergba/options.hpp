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

#ifndef LIBSHRINKLERGBA_OPTIONS_HPP
#define LIBSHRINKLERGBA_OPTIONS_HPP

#include "libshrinkler/libshrinkler.hpp"

namespace libshrinklergba
{

class options
{
public:
    bool verbose() const { return m_verbose; }
    void verbose(bool verbose) { m_verbose = verbose; }

    const libshrinkler::shrinkler_parameters& shrinkler_parameters() const { return m_shrinkler_parameters; }
    libshrinkler::shrinkler_parameters& shrinkler_parameters() { return m_shrinkler_parameters; }
    void shrinkler_parameters(const libshrinkler::shrinkler_parameters& p) { m_shrinkler_parameters = p; }

private:
    bool m_verbose = false;
    libshrinkler::shrinkler_parameters m_shrinkler_parameters;
};

}

#endif
