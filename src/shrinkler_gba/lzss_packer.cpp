// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module shrinkler_gba;

namespace shrinkler_gba
{

namespace
{

std::vector<unsigned char> lzss_compress(const std::vector<unsigned char>& /*data*/)
{
	// TODO: compress and return data
	return {};
}

}

cartridge lzss_packer::pack(const input_file& /*input_file*/)
{
	// TODO: real implementation
	//       * pack
	//         * lzss (move from pack.cpp)
	//         * huffman (move from pack.cpp)
	//       * assemble
	return {};
}

}
