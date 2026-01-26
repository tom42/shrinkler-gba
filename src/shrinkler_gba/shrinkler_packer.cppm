// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

export module shrinkler_gba:shrinkler_packer;
import :input_file;
import :options;
import :packer;
import libshrinkler;

namespace shrinkler_gba
{

class shrinkler_packer final : public packer
{
public:
    virtual cartridge pack(const input_file& input_file, const options& options) override;
};

}
