// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

export module shrinkler_gba:shrinkler_packer;
import :input_file;
import :packer;
import libshrinkler;

namespace shrinkler_gba
{

// TODO: this can go into the cpp file?
struct shrinkler_packer_options final
{
    bool code_in_header = true;
    bool debug_checks = false;
    libshrinkler::encoder_parameters encoder_parameters{};
};

class shrinkler_packer final : public packer
{
public:
    virtual cartridge pack(const input_file& input_file, const options& options) override;
};

}
