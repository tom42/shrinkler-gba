// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <string>
#include <vector>

export module shrinkler_gba:table_printer;

namespace shrinkler_gba
{

class table_printer final
{
public:
private:
    int m_table_indent = 0;
    std::vector<std::vector<std::string>> m_rows;
    std::vector<size_t> m_column_widths;
};

}
