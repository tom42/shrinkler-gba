// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

export module shrinkler_gba:lzss_packer;
import :packer;

namespace shrinkler_gba
{

class lzss_packer final : public packer
{
public:
	virtual cartridge pack(const input_file& input_file) override;
};

}
