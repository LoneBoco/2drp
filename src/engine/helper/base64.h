#pragma once

#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cctype>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>

namespace tdrp::base64
{

static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

static inline bool is_base64(uint8_t c)
{
	return (std::isalnum(c) || (c == '+') || (c == '/'));
}

std::vector<uint8_t> from(const std::string& input)
{
	std::vector<uint8_t> result;
	result.reserve(input.size() / 4 * 3);

	int byte_pos = 0;
	uint8_t bytes[4];
	for (auto iter = input.begin(); iter != input.end() && *iter != '='; ++iter)
	{
		if (std::isspace(*iter))
			continue;
		if (!is_base64(*iter))
			break;

		bytes[byte_pos++] = (uint8_t)base64_chars.find(*iter);

		if (byte_pos == 4)
		{
			byte_pos = 0;
			result.push_back( (bytes[0]	       << 2) + ((bytes[1] & 0x30) >> 4));
			result.push_back(((bytes[1] & 0xF) << 4) + ((bytes[2] & 0x3C) >> 2));
			result.push_back(((bytes[2] & 0x3) << 6) + ( bytes[3]));
		}
	}

	// Remainder bytes.
	if (byte_pos != 0)
	{
		for (int i = byte_pos; i < 4; ++i)
			bytes[i] = 0;

		result.push_back( (bytes[0]        << 2) + ((bytes[1] & 0x30) >> 4));
		result.push_back(((bytes[1] & 0xF) << 4) + ((bytes[2] & 0x3C) >> 2));
	}

	return result;
}

template <typename T>
std::string to(const T& input)
{
	using namespace boost::archive::iterators;

	typedef insert_linebreaks<base64_from_binary<transform_width<decltype(std::begin(input)), 6, 8>>, 72> iter_to_base64;
	auto padding = 3 - (std::size(input) % 3);

	std::string result{ iter_to_base64(std::begin(input)), iter_to_base64(std::end(input)) };
	result.append(padding, '=');

	return result;
}

} // end namespace tdrp::base64
