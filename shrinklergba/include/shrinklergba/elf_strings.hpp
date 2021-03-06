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

#ifndef SHRINKLERGBA_ELF_STRINGS_HPP
#define SHRINKLERGBA_ELF_STRINGS_HPP

#include <string>
#include "fmt/core.h"
#include "shrinklergba/elfio_wrapper.hpp"

namespace shrinklergba::elf_strings
{

template <typename TValue>
std::string to_hex(TValue value)
{
    return fmt::format("{:#0x}", value);
}

template <typename TValue, typename NDigits>
std::string to_hex(TValue value, NDigits ndigits)
{
    const auto prefix_width = 2;
    return fmt::format("{:#0{}x}", value, ndigits + prefix_width);
}

std::string get_section_type(ELFIO::Elf_Word type);
std::string get_section_flags(ELFIO::Elf_Xword flags);
std::string get_segment_type(ELFIO::Elf_Word type);
std::string get_segment_flags(ELFIO::Elf_Word flags);

}

#endif
