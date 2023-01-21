// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <algorithm>
#include <iostream>
#include <utility>
#include "shrinklergbacore/table_printer.hpp"

namespace shrinklergbacore
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
