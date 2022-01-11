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

#include <algorithm>
#include <iostream>
#include <utility>
#include "shrinklergba/table_printer.hpp"

namespace shrinklergba
{

template <typename NChars>
static void print_nchars(std::ostream& os, char c, NChars n)
{
    while (n--)
    {
        os << c;
    }
}

void table_printer::add_row(std::vector<std::string> row)
{
    for (size_t column = 0; column < row.size(); ++column)
    {
        if (column >= m_column_widths.size())
        {
            m_column_widths.push_back(row[column].size());
        }
        else
        {
            m_column_widths[column] = std::max(row[column].size(), m_column_widths[column]);
        }
    }

    m_rows.push_back(std::move(row));
}

void table_printer::print(std::ostream& os)
{
    for (const auto& row : m_rows)
    {
        // TODO: indentation before first row?
        indent_table(os);

        for (size_t column = 0; column < row.size(); ++column)
        {
            os << row[column];
            if (column < row.size() - 1)
            {
                pad_column(os, row[column], m_column_widths[column]);
                print_column_separator(os);
            }
        }

        os << std::endl;
    }
}

void table_printer::indent_table(std::ostream& os)
{
    print_nchars(os, ' ', table_indent());
}

void table_printer::pad_column(std::ostream& os, const std::string& column_text, size_t column_width)
{
    const auto npadding_chars = column_width - column_text.size();
    print_nchars(os, ' ', npadding_chars);
}

void table_printer::print_column_separator(std::ostream& os)
{
    os << ' ';
}

}
