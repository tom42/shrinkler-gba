// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <filesystem>
#include <optional>

export module shrinkler_gba:options;
import libshrinkler;

namespace shrinkler_gba
{

// TODO: turn this into a struct: it's just a bunch of data with no real invariant
export class options final
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
            m_output_file = input_file.filename().replace_extension("gba");
        }
    }

    const std::filesystem::path& output_file() const { return m_output_file; }

    void output_file(const std::filesystem::path& output_file)
    {
        m_output_file = output_file;
        m_output_file_set = true;
    }

    bool code_in_header() const { return m_code_in_header; }

    void code_in_header(bool code_in_header) { m_code_in_header = code_in_header; }

    bool debug_checks() const { return m_debug_checks; }

    void debug_checks(bool debug_checks) { m_debug_checks = debug_checks; }

    const std::optional<std::string>& packer() const { return m_packer; }

    void packer(std::optional<std::string> packer) { m_packer = std::move(packer); }

    const libshrinkler::encoder_parameters& shrinkler_parameters() const { return m_shrinkler_parameters; }

    libshrinkler::encoder_parameters& shrinkler_parameters() { return m_shrinkler_parameters; }

    void shrinkler_parameters(const libshrinkler::encoder_parameters& p) { m_shrinkler_parameters = p; }

private:
    bool m_verbose = false;
    bool m_output_file_set = false;
    std::filesystem::path m_input_file;
    std::filesystem::path m_output_file;
    bool m_code_in_header = true;
    bool m_debug_checks = false;
    std::optional<std::string> m_packer;
    libshrinkler::encoder_parameters m_shrinkler_parameters;
};

}
