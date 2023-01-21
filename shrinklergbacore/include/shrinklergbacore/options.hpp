// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_OPTIONS_HPP
#define SHRINKLERGBACORE_OPTIONS_HPP

#include <filesystem>
#include "shrinklerwrapper/shrinklerwrapper.hpp"

namespace shrinklergbacore
{

class options final
{
public:
    bool verbose() const { return m_verbose; }

    void verbose(bool verbose) { m_verbose = verbose; }

    const std::filesystem::path& input_file() const { return m_input_file; }

    void input_file(const std::filesystem::path& input_file)
    {
        m_input_file = input_file;

        if (!m_output_file_set)
        {
            m_output_file = input_file;
            m_output_file.replace_extension("gba");
        }
    }

    const std::filesystem::path& output_file() const { return m_output_file; }

    void output_file(const std::filesystem::path& output_file)
    {
        m_output_file = output_file;
        m_output_file_set = true;
    }

    const shrinklerwrapper::shrinkler_parameters& shrinkler_parameters() const { return m_shrinkler_parameters; }

    shrinklerwrapper::shrinkler_parameters& shrinkler_parameters() { return m_shrinkler_parameters; }

    void shrinkler_parameters(const shrinklerwrapper::shrinkler_parameters& p) { m_shrinkler_parameters = p; }

private:
    bool m_verbose = false;
    bool m_output_file_set = false;
    std::filesystem::path m_input_file;
    std::filesystem::path m_output_file;
    shrinklerwrapper::shrinkler_parameters m_shrinkler_parameters;
};

}

#endif
