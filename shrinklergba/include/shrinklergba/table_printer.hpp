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

#ifndef SHRINKLERGBA_TABLE_PRINTER_HPP
#define SHRINKLERGBA_TABLE_PRINTER_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace shrinklergba
{

class table_printer
{
public:
    void add_row(std::vector<std::string> row);
    void print(std::ostream& os);

    int table_indent() const { return m_table_indent; }
    void table_indent(int table_indent) { m_table_indent = table_indent; }

private:
    void indent_table(std::ostream& os);
    static void pad_column(std::ostream& os, const std::string& column_text, size_t column_width);
    static void print_column_separator(std::ostream& os);

    int m_table_indent = 0;
    std::vector<std::vector<std::string>> m_rows;
    std::vector<size_t> m_column_widths;
};

}

#endif
