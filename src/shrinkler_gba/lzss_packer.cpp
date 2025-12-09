// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <iterator>
#include <lzasm/arm/arm32/divided_thumb_assembler.hpp>
#include <vector>

module shrinkler_gba;
import agbpack;

namespace shrinkler_gba
{

using namespace std::literals::string_literals;

namespace
{

class lzss_cartridge_assembler final : private cartridge_assembler
{
public:
    lzss_cartridge_assembler()
    {
        m_data = assemble();
        // TODO: assemble stuff
        // TODO: write complement
        // TODO: final assertion: fixed byte
        // TODO: final assertion: complement
    }

    const std::vector<unsigned char>& data() const
    {
        return m_data;
    }

private:
    std::vector<unsigned char> assemble()
    {
        // TODO: later, honor the --no-code-in-header option
        //       * For starters we do NOT stick code into the header
        arm_branch("code_start"s);
        emit_nintendo_logo();
        emit_remaining_header();

    label("code_start"s);
        label("here"s).b("here"s); // TODO: endless loop, remove

        return link(mem_rom);
    }

    std::vector<unsigned char> m_data;
};

std::vector<unsigned char> lzss_compress(const std::vector<unsigned char>& input)
{
    std::vector<unsigned char> output;
    agbpack::optimal_lzss_encoder lzss_encoder;
    lzss_encoder.encode(input.begin(), input.end(), back_inserter(output));
    return output;
}

std::vector<unsigned char> huffman_compress(const std::vector<unsigned char>& input)
{
    std::vector<unsigned char> output;
    agbpack::huffman_encoder huffman_encoder;
    huffman_encoder.options(agbpack::huffman_options::h4);
    huffman_encoder.encode(input.begin(), input.end(), back_inserter(output));
    return output;
}

std::vector<unsigned char> compress(const std::vector<unsigned char>& input)
{
    return huffman_compress(lzss_compress(input));
}

cartridge assemble_cartridge(const std::vector<unsigned char>& compressed_binary)
{
    // TODO: replace std::vector<unsigned char> by something simple
    // TODO: assemble and return real cartridge
    // TODO: which of the options from the command line do we respect?
    //       * --debug-checks? (probably no?)
    //       * --no-code-in-header (probably yes?)
    // TODO: basically we
    //       * decode huffman into a temporary buffer
    //       * decode LZSS into the final location
    //       * where IS the temporary buffer?
    //         * end of EWRAM?
    //         * middle of EWRAM? Advantage: most likely we'll depack to the beginning of IWRAM or EWRAM, so middle should not intefere too much
    //         * If the entry point is in IWRAM, use EWRAM as tmp buffer
    //         * If the entry point is in EWRAM, use IWRAM as tmp buffer
    lzss_cartridge_assembler assembler;
    return cartridge
    {
        .data = assembler.data(),
        .compressed_size = compressed_binary.size()
        // TODO: fill in depacker size
    };
}

}

cartridge lzss_packer::pack(const input_file& input_file)
{
    auto compressed_binary = huffman_compress(lzss_compress(input_file.data()));
    return assemble_cartridge(compressed_binary);
}

}
