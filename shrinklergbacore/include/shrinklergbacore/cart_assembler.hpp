// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_CART_ASSEMBLER_HPP
#define SHRINKLERGBACORE_CART_ASSEMBLER_HPP

#include <string>
#include <vector>
#include "lzasm/arm/arm32/divided_thumb_assembler.hpp"
#include "shrinklergbacore/input_file.hpp"

namespace shrinklergbacore
{

class depacker_settings final
{
public:
    bool debug_checks = false;
};

// TODO: stick code into header, but turn it around first so that uninteresting bits are there That is:
//       * Turn around depacker code, so that the non-changing bits are at the beginning (AND CHECK THIS IN)
//         * Do not forget that since the entry point is ARM code we most also move the align directive
//           * This wastes then potentially 2 bytes. But we could even warn this case and throw, and worry about it later...
//       * Then move these bits into the header.
class cart_assembler final : private lzasm::arm::arm32::divided_thumb_assembler
{
public:
    cart_assembler(const input_file& input_file, const std::vector<unsigned char>& compressed_program, const depacker_settings& settings);

    const std::vector<unsigned char>& data() const
    {
        return m_data;
    }

    size_t depacker_size() const
    {
        return m_depacker_size;
    }

private:
    void write_complement();
    std::vector<unsigned char> assemble(const input_file& input_file, const std::vector<unsigned char>& compressed_program, bool debug);

    void emit_nintendo_logo();
    void emit_remaining_header();

    // Emit zero terminated string, similar to the GNU assembler's .asciz directive.
    // The terminating zero is also emitted.
    void asciz(const std::string& s);

    // Macro that calls the panic routine if the size of the decompressed data is incorrect.
    // This macro expects outp (the output pointer) to point to the byte after the last decompressed byte.
    // This macro clobbers all registers except sp.
    void debug_check_decompressed_data_size(const input_file& input_file, bool debug);

    // Macro that calls the panic routine if the checksum of the decompressed data is incorrect.
    // This macro clobbers all registers except sp.
    void debug_check_decompressed_data(const input_file& input_file, bool debug);

    // Macro that calls the panic routine if the stack pointer has not been restored to its initial value.
    // This macro clobbers all registers.
    void debug_check_sp_on_exit(bool debug);

    // Macro that calls the panic routine. The panic routine clobbers all registers and does not return.
    void debug_call_panic_routine(bool debug, const std::string& message);

    // Macro that emits the panic routine. The panic routine clobbers all registers and does not return.
    // The panic routine must be called with r2 pointing to a zero terminated error message.
    // This message will be printed using Mappy / VisualBoyAdvance debug output.
    void debug_emit_panic_routine(bool debug);

    void throw_if_wrong_lc(lzasm::arm::arm32::address_t expected_lc, const char* what) const;
    void throw_if_not_aligned(lzasm::arm::arm32::address_t alignment) const;
    void throw_if_fixed_byte_wrong() const;
    void throw_if_complement_wrong() const;

    const depacker_settings settings;
    std::vector<unsigned char> m_data;
    size_t m_depacker_size;
};

}

#endif
