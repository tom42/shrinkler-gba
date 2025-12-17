// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstddef>
#include <lzasm/arm/arm32/divided_thumb_assembler.hpp>
#include <string>
#include <vector>

export module shrinkler_gba:shrinkler_packer;
import :input_file;
import :packer;

namespace shrinkler_gba
{

class shrinkler_packer final : public packer
{
public:
    // TODO: add ctor
    // TODO: add pack()
private:
};

}

// TODO: redesign stuff below: the assembler will be hidden
//       * review headers (in particular, LZASM probably does not need to be here anymore)
namespace shrinkler_gba
{

// TODO: rename to shrinkler_(de)packer_options
//       * likewise, rename "settings" to options where applicable
struct shrinkler_depacker_settings final
{
    bool code_in_header = true;
    bool debug_checks = false;
};

class shrinkler_cartridge_assembler final : private cartridge_assembler
{
public:
    shrinkler_cartridge_assembler(const input_file& input_file, const bytevector& compressed_program, const shrinkler_depacker_settings& settings);

    const bytevector& data() const
    {
        return m_data;
    }

    size_t depacker_size() const
    {
        return m_depacker_size;
    }

private:
    bytevector assemble(const input_file& input_file, const bytevector& compressed_program);

    // Macro that calls the panic routine if the size of the decompressed data is incorrect.
    // This macro expects outp (the output pointer) to point to the byte after the last decompressed byte.
    // This macro clobbers all registers except sp.
    void debug_check_decompressed_data_size(const input_file& input_file);

    // Macro that calls the panic routine if the checksum of the decompressed data is incorrect.
    // This macro clobbers all registers except sp.
    void debug_check_decompressed_data(const input_file& input_file);

    // Macro that calls the panic routine if the stack pointer has not been restored to its initial value.
    // This macro clobbers all registers.
    void debug_check_sp_on_exit();

    // Macro that calls the panic routine. The panic routine clobbers all registers and does not return.
    void debug_call_panic_routine(const std::string& message);

    // Macro that emits the panic routine. The panic routine clobbers all registers and does not return.
    // The panic routine must be called with r2 pointing to a zero terminated error message.
    // This message will be printed using Mappy / VisualBoyAdvance debug output.
    void debug_emit_panic_routine();

    const shrinkler_depacker_settings m_settings;
    bytevector m_data;
    size_t m_depacker_size{};
};

}
