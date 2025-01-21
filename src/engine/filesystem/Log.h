#pragma once

#include "common.h"

// Don't change this order.  For some reason the compile will fail.
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
//

#include <array>
#include <format>
#include <string_view>


namespace tdrp::log
{

inline std::ofstream& get_file()
{
	static std::ofstream m_file{};
	return m_file;
}

inline bool& get_wrote_newline()
{
	static bool m_wrote_newline = true;
	return m_wrote_newline;
}


template <typename ...Args>
void Print(std::string_view fmt, const Args&... args)
{
	std::ostringstream text;

	if (get_wrote_newline())
	{
		auto localtime = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() }.get_local_time();
		text << std::format("[{:%F %H:%M:%S}] ", std::chrono::floor<std::chrono::seconds>(localtime));
		get_wrote_newline() = false;
	}

	text << std::vformat(fmt, std::make_format_args(args...));

	auto s = text.str();
	if (s.size() == 0)
		return;

	if (get_file().is_open())
		get_file() << s;

	std::cout << s;

	if (s.back() == '\n')
		get_wrote_newline() = true;
}

template <typename ...Args>
void PrintLine(std::string_view fmt, const Args&... args)
{
	Print(fmt, args...);
	Print("\n");
}

template <uint32_t N>
constexpr std::string Indent()
{
	if constexpr (N == 0)
	{
		static std::array<char, 1> result = { '\0' };
		return std::string{ result.data() };
	}
	else
	{
		constexpr uint32_t size = 4 + (N * 2) + 1;
		std::array<char, size> result;
		result.fill(' ');
		result.back() = '\0';
		return std::string{ result.data() };
	}
}

inline void SetLogFile(const std::filesystem::path& p)
{
	if (get_file() && get_file().is_open())
	{
		get_file().flush();
		get_file().close();
	}
	get_file().open(p, std::ios::binary | std::ios::out | std::ios::app);
}

} // end namespace log
