// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

// Simple logging/console output that can be enabled or disabled.
// Regarding wrapping std::format, see https://stackoverflow.com/questions/72795189/how-can-i-wrap-stdformat-with-my-own-template-function.
// Note: std::format_string is basically a wrapper around a string view, so it can and should be passed by value.

module;

#include <cstdio>
#include <format>
#include <print>
#include <utility>

export module shrinkler_gba:console;

namespace shrinkler_gba
{

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
class console final
{
public:
    template<typename... Args>
    void out(std::format_string<Args...> fmt, Args&&... args) const
    {
        if (m_out_stream)
        {
            std::println(m_out_stream, fmt, std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    void warn(std::format_string<Args...> fmt, Args&&... args) const
    {
        if (m_warn_stream)
        {
            std::print(m_warn_stream, "Warning: ");
            std::println(m_warn_stream, fmt, std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    void verbose(std::format_string<Args...> fmt, Args&&... args) const
    {
        if (m_verbose_stream)
        {
            std::println(m_verbose_stream, fmt, std::forward<Args>(args)...);
        }
    }

    void out_stream(FILE* stream)
    {
        m_out_stream = stream;
    }

    void warn_stream(FILE* stream)
    {
        m_warn_stream = stream;
    }

    void verbose_stream(FILE* stream)
    {
        m_verbose_stream = stream;
    }

    bool is_verbose_enabled() const
    {
        return m_verbose_stream;
    }

private:
    FILE* m_out_stream = stdout;
    FILE* m_warn_stream = stdout;
    FILE* m_verbose_stream = stdout;
};

}
