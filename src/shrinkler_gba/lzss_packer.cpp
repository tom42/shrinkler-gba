// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <gsl/gsl>
#include <iterator>
#include <lzasm/arm/arm32/divided_thumb_assembler.hpp>
#include <stdexcept>
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
    lzss_cartridge_assembler(const input_file& input_file, const bytevector& compressed_binary, const lzss_packer_options& options)
    {
        m_data = assemble(input_file, compressed_binary, options);

        write_complement(m_data, options.code_in_header ? ofs_complement_if_code_in_header : ofs_complement);

        throw_if_fixed_byte_wrong(m_data);
        throw_if_complement_wrong(m_data);
    }

    const bytevector& data() const
    {
        return m_data;
    }

    size_t depacker_size() const
    {
        return m_depacker_size;
    }

private:
    bytevector assemble(const input_file& input_file, const bytevector& compressed_binary, const lzss_packer_options& options)
    {
        arm_branch("code_start"s);
        emit_nintendo_logo();
        if (!options.code_in_header)
        {
            emit_remaining_header();
        }

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
        ldr(r1, gsl::narrow<int32_t>(input_file.load_address()));
        if (options.code_in_header)
        {
            // Fixed byte of value 0x96, followed by unit code which can be freely chosen.
            // We insert an instruction here that does not bother us and stomp over it.
            throw_if_wrong_lc(ofs_fixed_byte, "fixed byte");
            mov(r2, fixed_byte_value);
        }
        swi(swi_lz77_uncomp_wram);

        // Branch to entry point
        ldr(r0, gsl::narrow<int32_t>(input_file.entry()));
        bx(r0);

        if (options.code_in_header)
        {
            // Our depacker code is so short that if we put it into the ROM header it won't
            // even go past the game version byte. The game version byte is somewhere in
            // the literal pool below, and obviously we do not want anything overwritten
            // there by the checksum. Therefore we reserve a halfword here, of which we
            // later patch a byte to get a valid checksum. Luckily this halfword is also
            // needed to align the literal pool below, so it is not even wasting space.
            throw_if_wrong_lc(ofs_complement_if_code_in_header, "complement if depacker code is in ROM header");
            hword(0xffff);
            throw_if_wrong_lc(ofs_game_version, "game version"); // Sanity check: game version really is in the pool
        }

        throw_if_not_aligned(2);
        pool();

        ////////////////////////////////////////////////////////////////////////
        // Compressed intro.
        // Must be word aligned, since
        // * The BIOS needs it that way
        // * We use adr to load the address to the packed data,
        //   which requires word alignment.
        ////////////////////////////////////////////////////////////////////////
        throw_if_not_aligned(2);
        m_depacker_size = current_lc() - gba_header_size;
    label("packed_intro"s);
        incbin(compressed_binary.begin(), compressed_binary.end());

        return link(mem_rom);
    }

    static constexpr size_t ofs_complement_if_code_in_header = ofs_game_version - 2;
    bytevector m_data;
    size_t m_depacker_size{};
};

bytevector lzss_compress(const bytevector& input)
{
    bytevector output;
    agbpack::optimal_lzss_encoder encoder;
    encoder.vram_safe(false);
    encoder.encode(input.begin(), input.end(), back_inserter(output));

    bytevector verified;
    agbpack::lzss_decoder decoder;
    decoder.vram_safe(false);
    decoder.decode(output.begin(), output.end(), back_inserter(verified));
    if (verified != input)
    {
        throw internal_error("verification of LZSS encoded data failed");
    }

    return output;
}

bytevector huffman_compress(const bytevector& input)
{
    bytevector output;
    agbpack::huffman_encoder encoder;
    encoder.options(agbpack::huffman_options::h4);
    encoder.encode(input.begin(), input.end(), back_inserter(output));

    bytevector verified;
    agbpack::huffman_decoder decoder;
    decoder.decode(output.begin(), output.end(), back_inserter(verified));
    if (verified != input)
    {
        throw internal_error("verification of huffman encoded data failed");
    }

    return output;
}

bytevector compress(const bytevector& input)
{
    return huffman_compress(lzss_compress(input));
}

cartridge assemble_cartridge(const input_file& input_file, const bytevector& compressed_binary, const lzss_packer_options& options)
{
    // TODO: basically we
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
    auto compressed_binary = compress(input_file.data());
    return assemble_cartridge(input_file, compressed_binary, m_options);
}

}
