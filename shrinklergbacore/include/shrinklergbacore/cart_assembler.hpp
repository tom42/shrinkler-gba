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

// TODO: stick code into header, but turn it around first so that uninteresting bits are there That is:
//       * Turn around depacker code, so that the non-changing bits are at the beginning (AND CHECK THIS IN)
//         * Do not forget that since the entry point is ARM code we most also move the align directive
//           * This wastes then potentially 2 bytes. But we could even warn this case and throw, and worry about it later...
//       * Then move these bits into the header.
class cart_assembler final : private lzasm::arm::arm32::divided_thumb_assembler
{
public:
    cart_assembler(const input_file& input_file, const std::vector<unsigned char>& compressed_program);

    const std::vector<unsigned char>& data() const
    {
        return m_data;
    }

    size_t depacker_size() const
    {
        return m_depacker_size;
    }

private:
    std::vector<unsigned char> assemble(const input_file& input_file, const std::vector<unsigned char>& compressed_program, bool debug);
    void write_complement();

    // Macro that calls the panic routine. The panic routine clobbers all registers and does not return.
    void debug_call_panic_routine(bool debug, const std::string& message);

    // Macro that emits the panic routine. The panic routine clobbers all registers and does not return.
    // The panic routine must be called with r2 pointing to a zero terminated error message.
    // This message will be printed using Mappy / VisualBoyAdvance debug output.
    void debug_emit_panic_routine(bool debug);

    std::vector<unsigned char> m_data;
    size_t m_depacker_size;
};

}

#endif
