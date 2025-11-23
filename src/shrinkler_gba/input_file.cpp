// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <elfio/elfio.hpp>
#include <fstream>
#include <stdexcept>
#include <string>
#include <system_error>

module shrinkler_gba;

namespace shrinkler_gba
{

using ELFIO::elfio;

namespace
{

void open_elf(elfio& reader, std::istream& stream)
{
    if (!reader.load(stream))
    {
        throw std::runtime_error("File is not a valid ELF file");
    }
}

}

void input_file::load(const std::string& path)
{
    try
    {
        // TODO: originally we logged here, question is, do we really want this? Not really, no?
        //       => Well probably we do - if the program crashes we want to be able to read what it was doing, no?
        std::ifstream stream(path, std::ios::binary);

        if (!stream)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        load(stream);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(path + ": " + e.what());
    }
}

void input_file::load(std::istream& stream)
{
    load_elf(stream);

    // TODO: log data (see old implementation): entry, load address, total size of loaded data
}

void input_file::load_elf(std::istream& stream)
{
    reset();

    elfio reader;
    open_elf(reader, stream);

    // TODO: implement stuff below
    /*
    check_header(reader);
    read_entry(reader);
    log_program_headers(reader);
    log_section_headers(reader);
    convert_to_binary(reader);
    */
}

void input_file::reset()
{
    m_entry = 0;
    m_load_address = 0;
    std::vector<unsigned char>().swap(m_data);
}

}
