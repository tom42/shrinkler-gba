// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <filesystem>
#include <optional>

export module shrinkler_gba:options;
import libshrinkler;

namespace shrinkler_gba
{

// TODO: before doing the thing below, get rid of any logic in here:
//       * Get rid of m_output_file_set. Either have the output file be an optional<path>, or just check whether it is empty
//         * Avoiding optional is preferable here, since argpppp does not really support optional.
//           * We could probably rather easily give it support for value<optional<string>>
//           * On the other hand we could also say that from the point of pack.cpp's point of view,
//             the output file name is not optional and it is up to command line parsing to supply a default.
//             * For the time being I'd probably prefer that solution
//       * Then remove logic from input_file. We can either post process this in command_line.cpp(m), or we do it directly in pack.cpp. It does not matter much
// TODO: turn this into a struct: it's just a bunch of data with no real invariant
//       => We can do this, but we need support for --no-code-in-header, which has inverted logic:
//       => There is a number of ways to implement this:
//          1) value<bool>() constructor gets a second argument, which is the value to write to the boolean variable
//          2) value<bool>() gets an inverted() (name?) method, so that one can write value(myvariable).inverted().
//             Here inverted means "write false to variable" when option is encountered
//          3) value<bool>() automatically guesses from the option name whether to write true or false to the variable:
//             * If the option's long name starts with --no, then write false to the variable
//             * If the option's long name does not start with --no, then write true to the variable
//             * What if the option does not have a long name? => Well then assume it is not called --no? That might work, no?
//          4) As a workaround, use callback<bool>, which always works, no?
//       => What is the most universally useful variant? We should use that, anything else can be tacked on top using helper functions
//       => Also: consider whether argpppp::value should also be specialized for std::optional.
//          => That is: if we have specialized e.g. value<string>, then we should also have value<optional<string>>
//          => Likewise, if we have specialized value<signed_integral>, then we should also have value<optional<signed_integral>>
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
