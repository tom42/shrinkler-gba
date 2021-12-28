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

#ifndef SHRINKLERGBA_CONSOLE_HPP
#define SHRINKLERGBA_CONSOLE_HPP

#include <iostream>

namespace shrinklergba
{

class console
{
public:
    std::ostream* out() const { return m_out; }
    void out(std::ostream* s) { m_out = s; }
    bool is_out_enabled() const { return out(); }

    std::ostream* verbose() const { return m_verbose; }
    void verbose(std::ostream* s) { m_verbose = s; }
    bool is_verbose_enabled() const { return verbose(); }

private:
    std::ostream* m_out = &std::cout;
    std::ostream* m_verbose = &std::cout;
};

#define CONSOLE_OUT(console)                \
    if (!(console).is_out_enabled());       \
        else *(console).out()

#define CONSOLE_VERBOSE(console)            \
    if (!(console).is_verbose_enabled());   \
        else *(console).verbose()

}

#endif
