// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_CONSOLE_HPP
#define SHRINKLERGBACORE_CONSOLE_HPP

#include <iostream>

namespace shrinklergbacore
{

class console final
{
public:
    std::ostream* out() const { return m_out; }
    void out(std::ostream* s) { m_out = s; }
    bool is_out_enabled() const { return out(); }

    std::ostream* verbose() const { return m_verbose; }
    void verbose(std::ostream* s) { m_verbose = s; }
    bool is_verbose_enabled() const { return verbose(); }

private:
    std::ostream* m_out = &std::cout;
    std::ostream* m_verbose = &std::cout;
};

#define CONSOLE_OUT(console)                \
    if (!(console).is_out_enabled());       \
        else *(console).out()

#define CONSOLE_VERBOSE(console)            \
    if (!(console).is_verbose_enabled());   \
        else *(console).verbose()

}

#endif
