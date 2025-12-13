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
    lzss_cartridge_assembler(const input_file& input_file, const std::vector<unsigned char>& compressed_binary, const lzss_packer_options& options)
    {
        m_data = assemble(input_file, compressed_binary, options);

        write_complement(m_data, ofs_complement);

        throw_if_fixed_byte_wrong(m_data);
        throw_if_complement_wrong(m_data);
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
    std::vector<unsigned char> assemble(const input_file& input_file, const std::vector<unsigned char>& compressed_binary, const lzss_packer_options& /*options*/)
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

        // Decompress data using BIOS functions.
        // Note that the BIOS functions modify r0 and r1, so don't try to be smart with reusing register contents.

        // TODO: that's only to get started: we depack to start of EWRAM, so we can see what we're doing
        //       * We need to decode somewhere where we're not interfering with the load address. A safe bet might be end of EWRAM
        //       * We should have a runtime check that this is actually the case. Problem here is that we can't just fail, because
        //         ultimately we want to pick the smallest of a couple of compression mechanisms, so we can't just die if something
        //         is not right.
        const auto lzss_packed_data = mem_ewram;

        // Huffman decode to temporary buffer
        adr(r0, "packed_intro"s);
        ldr(r1, lzss_packed_data);
        swi(swi_huff_uncomp);

        // LZSS decode to load address
        ldr(r0, lzss_packed_data);
        ldr(r1, input_file.load_address());
        swi(swi_lz77_uncomp_wram);

        // Branch to entry point
        ldr(r0, input_file.entry());
        bx(r0);

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
    // TODO: also verify: verification should really be something provided by agbpack, but since it does not yet do that we'll do it here
    std::vector<unsigned char> output;
    agbpack::optimal_lzss_encoder lzss_encoder;
    lzss_encoder.vram_safe(false);
    lzss_encoder.encode(input.begin(), input.end(), back_inserter(output));
    return output;
}

std::vector<unsigned char> huffman_compress(const std::vector<unsigned char>& input)
{
    // TODO: also verify: verification should really be something provided by agbpack, but since it does not yet do that we'll do it here
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

cartridge assemble_cartridge(const input_file& input_file, const std::vector<unsigned char>& compressed_binary, const lzss_packer_options& options)
{
    // TODO: replace std::vector<unsigned char> by something simple
    // TODO: basically we
    //       * decode huffman into a temporary buffer
    //       * decode LZSS into the final location
    //       * where IS the temporary buffer?
    //         * end of EWRAM?
    //         * middle of EWRAM? Advantage: most likely we'll depack to the beginning of IWRAM or EWRAM, so middle should not intefere too much
    //         * If the entry point is in IWRAM, use EWRAM as tmp buffer
    //         * If the entry point is in EWRAM, use IWRAM as tmp buffer
    lzss_cartridge_assembler assembler(input_file, compressed_binary, options);
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
    return assemble_cartridge(input_file, compressed_binary, m_options);
}

}
