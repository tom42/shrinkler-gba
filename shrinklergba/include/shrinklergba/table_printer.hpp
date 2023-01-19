// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBA_TABLE_PRINTER_HPP
#define SHRINKLERGBA_TABLE_PRINTER_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace shrinklergba
{

class table_printer final
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
