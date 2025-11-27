// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <string>
#include <vector>

export module shrinkler_gba:table_printer;
import :console;

namespace shrinkler_gba
{

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
class table_printer final
{
public:
    void add_row(std::vector<std::string> row);

    // Taking an std::ostream as argument here would be more sensible, but as of
    // November 2025 that causes some module related internal compiler error in MSVC.
    // As a workaround, take a console as argument.
    void print(console& c);

    int table_indent() const { return m_table_indent; }

    void table_indent(int table_indent) { m_table_indent = table_indent; }

private:
    int m_table_indent = 0;
    std::vector<std::vector<std::string>> m_rows;
    std::vector<size_t> m_column_widths;
};

}
