// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_TABLE_PRINTER_HPP
#define SHRINKLERGBACORE_TABLE_PRINTER_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace shrinklergbacore
{

class table_printer final
{
public:
private:
    void indent_table(std::ostream& os);
    static void pad_column(std::ostream& os, const std::string& column_text, size_t column_width);
    static void print_column_separator(std::ostream& os);
};

}

#endif
