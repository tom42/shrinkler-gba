// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <filesystem>
#include <optional>
#include <string>

export module shrinkler_gba:options;
import libshrinkler;

namespace shrinkler_gba
{

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
SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
struct options final
{
    bool verbose = false;
    std::filesystem::path input_file;
    std::filesystem::path output_file;
    bool code_in_header = true;
    bool debug_checks = false;
    std::optional<std::string> packer;
    libshrinkler::encoder_parameters shrinkler_parameters{};
};

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
inline bool has_output_file(const options& options)
{
    return !options.output_file.empty();
}

}
