// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <string_view>

module shrinkler_gba;

namespace shrinkler_gba
{

std::runtime_error internal_error(std::string_view s)
{
    std::string msg("INTERNAL ERROR: ");
    msg += s;
    return std::runtime_error(msg);
}

}
