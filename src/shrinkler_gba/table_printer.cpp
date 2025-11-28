// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <sstream>
#include <string>

module shrinkler_gba;

namespace shrinkler_gba
{

namespace
{

template <typename NChars>
static void print_nchars(std::ostream& os, char c, NChars n)
{
    while (n--)
    {
        os << c;
    }
}

void indent_table(std::ostream& os, int table_indent)
{
    print_nchars(os, ' ', table_indent);
}

void pad_column(std::ostream& os, const std::string& column_text, size_t column_width)
{
    const auto npadding_chars = column_width - column_text.size();
    print_nchars(os, ' ', npadding_chars);
}

void print_column_separator(std::ostream& os)
{
    os << ' ';
}

bool is_new_column(size_t column, const std::vector<size_t>& column_widths)
{
    return column >= column_widths.size();
}

bool is_last_column(size_t column, const std::vector<std::string>& row)
{
    return column >= row.size() - 1;
}

}

void table_printer::add_row(std::vector<std::string> row)
{
    for (size_t column = 0; column < row.size(); ++column)
    {
        if (is_new_column(column, m_column_widths))
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

void table_printer::print(const console& c)
{
    for (const auto& row : m_rows)
    {
        // Temporary workaround: see declaration of table_printer::print.
        // We need a temporary stream here to assemble the output.
        std::stringstream os;

        indent_table(os, table_indent());

        for (size_t column = 0; column < row.size(); ++column)
        {
            os << row[column];
            if (!is_last_column(column, row))
            {
                pad_column(os, row[column], m_column_widths[column]);
                print_column_separator(os);
            }
        }

        c.verbose(os.str());
    }
}

}
