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
