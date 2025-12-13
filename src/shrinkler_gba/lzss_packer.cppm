// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

export module shrinkler_gba:lzss_packer;
import :packer;

namespace shrinkler_gba
{

struct lzss_packer_options final
{
    bool code_in_header = true;
};

class lzss_packer final : public packer
{
public:
    lzss_packer(const lzss_packer_options& options) : m_options(options) {}

    virtual cartridge pack(const input_file& input_file) override;

private:
    lzss_packer_options m_options{};
};

}
