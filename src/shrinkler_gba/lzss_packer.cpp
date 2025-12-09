// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <iterator>
#include <vector>

module shrinkler_gba;
import agbpack;

namespace shrinkler_gba
{

namespace
{

std::vector<unsigned char> lzss_compress(const std::vector<unsigned char>& input)
{
	std::vector<unsigned char> output;
	agbpack::optimal_lzss_encoder lzss_encoder;
	lzss_encoder.encode(input.begin(), input.end(), back_inserter(output));
	return output;
}

std::vector<unsigned char> huffman_compress(const std::vector<unsigned char>& input)
{
	std::vector<unsigned char> output;
	agbpack::huffman_encoder huffman_encoder;
	huffman_encoder.options(agbpack::huffman_options::h4);
	huffman_encoder.encode(input.begin(), input.end(), back_inserter(output));
	return output;
}

}

cartridge lzss_packer::pack(const input_file& input_file)
{
	// TODO: real implementation
	//       * pack
	//         * lzss (move from pack.cpp)
	//         * huffman (move from pack.cpp)
	//       * assemble
	auto x = huffman_compress(lzss_compress(input_file.data())); // TODO: name: x
	return {};
}

}
