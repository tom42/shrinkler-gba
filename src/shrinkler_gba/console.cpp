// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <ostream>
#include <string>

module shrinkler_gba;

namespace shrinkler_gba
{

void console::verbose(const char* s)
{
    if (m_verbose_stream)
    {
        *m_verbose_stream << (s ? s : "(null)") << "\n";
    }
}

void console::verbose(const std::string& s)
{
    if (m_verbose_stream)
    {
        *m_verbose_stream << s << "\n";
    }
}

}
