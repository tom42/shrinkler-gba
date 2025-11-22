// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <fstream>
#include <stdexcept>
#include <string>

module shrinkler_gba;

namespace shrinkler_gba
{

void input_file::load(const std::string& path)
{
    try
    {
        // TODO: originally we logged here, question is, do we really want this? Not really, no?
        std::ifstream stream(path, std::ios::binary);

        if (!stream)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        // TODO: delegate to load() taking a stream
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(path + ": " + e.what());
    }
}

}
