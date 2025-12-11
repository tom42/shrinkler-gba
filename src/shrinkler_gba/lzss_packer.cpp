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

using namespace lzasm::arm::arm32;
using namespace std::literals::string_literals;

namespace
{

class lzss_cartridge_assembler final : private cartridge_assembler
{
public:
    lzss_cartridge_assembler(const std::vector<unsigned char>& compressed_binary)
    {
        m_data = assemble(compressed_binary);
        // TODO: write complement
        // TODO: final assertion: fixed byte
        // TODO: final assertion: complement
    }

    const std::vector<unsigned char>& data() const
    {
        return m_data;
    }

    size_t depacker_size() const
    {
        return m_depacker_size;
    }

private:
    std::vector<unsigned char> assemble(const std::vector<unsigned char>& compressed_binary)
    {
        // TODO: later, honor the --no-code-in-header option
        //       * For starters we do NOT stick code into the header
        arm_branch("code_start"s);
        emit_nintendo_logo();
        emit_remaining_header();

        // Entry point. Initially the GBA is in ARM state.
        // Immediately switch to Thumb state.
        throw_if_not_aligned(2);
        align(2);
    label("code_start"s);
        arm_to_thumb(r0);

        // Huffman decode
        adr(r0, "packed_intro"s);
        ldr(r1, mem_ewram); // TODO: that's only to get started: we depack to start of EWRAM, so we can see what we're doing
        swi(0x13);          // TODO: constant for swi number?

        // LZSS decode to load address
        mov(r0, r1);
        ldr(r1, mem_iwram); // TODO: unhardcode: that's the load address from input_file
        swi(0x11);          // TODO: constant for swi number?

        // Branch to entry point
        ldr(r0, mem_iwram); // TODO: unhardcode: that's the entry point from input_file
        bx(r0);

        // TODO: huffman decode to EWRAM (where to? => somewhere where it does not interfere with the load address)
        // TODO: lzss decode to load address
        pool();

        ////////////////////////////////////////////////////////////////////////
        // Compressed intro.
        // Must be word aligned, since
        // * The BIOS needs it that way
        // * We use adr to load the address to the packed data,
        //   which requires word alignment.
        ////////////////////////////////////////////////////////////////////////
        align(2);
        m_depacker_size = current_lc() - gba_header_size;
    label("packed_intro"s);
        incbin(compressed_binary.begin(), compressed_binary.end());

        return link(mem_rom);
    }

    std::vector<unsigned char> m_data;
    size_t m_depacker_size{};
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
    lzss_cartridge_assembler assembler(compressed_binary);
    return cartridge
    {
        .data = assembler.data(),
        .compressed_size = compressed_binary.size(),
        .depacker_size = assembler.depacker_size()
    };
}

}

cartridge lzss_packer::pack(const input_file& input_file)
{
    auto compressed_binary = huffman_compress(lzss_compress(input_file.data()));
    return assemble_cartridge(compressed_binary);
}

}
