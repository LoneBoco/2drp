#pragma once

#include "engine/common.h"
#include <mutex>
#include <array>
#include <format>

// Don't change this order.  For some reason the compile will fail.
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
//

using namespace std::literals;

namespace tdrp::log
{

// The timestamp mode for the log.
enum class TimestampMode
{
	None,

	// [HH:MM AM] (%I:%M %p)
	Short,

	// [yyyy-MM-dd HH:MM:SS] (%F %T)
	Long
};

inline constexpr std::string_view TimestampShort = "[{0:%I}:{0:%M} {0:%p}]"sv;
inline constexpr std::string_view TimestampLong = "[{0:%F} {0:%T}]"sv;

///////////////////////////////////////////////////////////////////////////////

struct is_absolute_t { explicit is_absolute_t() = default; };
inline constexpr is_absolute_t is_absolute{};

/// <summary>
/// 
/// </summary>
class Log;
struct Indent
{
	Indent(Log* log, uint8_t level);
	Indent(is_absolute_t is_absolute, Log* log, uint8_t level);
	Indent(Indent&& other) noexcept;
	~Indent() noexcept;

private:
	Log* m_log = nullptr;
	uint8_t m_old_level = 0;
};

///////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 
/// </summary>
struct Log
{
	std::filesystem::path filename;
	uint8_t indent_spaces = 2;
	uint8_t indent_level = 0;
	uint8_t indent_additional_spaces = 3;
	TimestampMode timestamp_file = TimestampMode::Long;
	TimestampMode timestamp_cli = TimestampMode::Short;
	bool mirror_to_cli = true;
	bool at_line_start = true;
	std::unique_ptr<std::ofstream> file;
	std::recursive_mutex mutex;

	// Reloads the log file (after a filename change).
	Log& reload();

	// Closes the log file.
	Log& close();

	// Clears the log file.
	Log& clear();

	// Gets the output file stream.
	std::ofstream* get_file();

	// Creates an RAII indentation object.
	Indent indent(uint8_t levels = 1)
	{
		return Indent(this, levels);
	}

	// Creates an RAII indentation object on an absolute indentation level.
	Indent indent_absolute(uint8_t level)
	{
		return Indent(is_absolute, this, level);
	}

	// Creates a unique pointer to an RAII indentation object.
	std::unique_ptr<Indent> indent_ptr(uint8_t levels = 1)
	{
		return std::make_unique<Indent>(this, levels);
	}

	// Returns if we are currently indented.
	bool not_indented() const
	{
		return indent_level == 0;
	}
};

///////////////////////////////////////////////////////////////////////////////

// The serverlog.txt file.
inline Log game{ .filename = std::filesystem::path{ "logs" } / "game.txt" };

///////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Prints a message to the log file and console.
/// </summary>
/// <param name="log"></param>
/// <param name="fmt"></param>
/// <param name="...args"></param>
template <typename ...Args>
void Print(Log& log, std::string_view fmt, const Args&... args)
{
	std::lock_guard lock(log.mutex);
	std::ostringstream text;

	// Add the indentation whitespace.
	uint8_t spaces = 0;
	if (log.at_line_start && log.indent_level != 0)
	{
		spaces = log.indent_additional_spaces + (log.indent_spaces * log.indent_level);
		text << std::string(spaces, ' ');
	}

	// Output the message.
	if constexpr (sizeof...(args) == 0)
		text << fmt;
	else
		text << std::vformat(fmt, std::make_format_args(args...));

	// Get the resultant string.
	// If empty, don't log anything.
	auto s = text.str();
	if (s.size() <= spaces)
		return;

	// Get the current time, floored to seconds.
	auto localtime = std::chrono::floor<std::chrono::seconds>(std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() }.get_local_time());

	// Output to file.
	if (auto* logFile = log.get_file(); logFile && logFile->is_open())
	{
		if (log.at_line_start && log.timestamp_file == TimestampMode::Short)
			*logFile << std::format(TimestampShort, localtime) << ' ';
		else if (log.at_line_start && log.timestamp_file == TimestampMode::Long)
			*logFile << std::format(TimestampLong, localtime) << ' ';

		*logFile << s;
		logFile->flush();
	}

	// Output to console.
	if (log.mirror_to_cli)
	{
		if (log.at_line_start && log.timestamp_cli == TimestampMode::Short)
			std::cout << std::format(TimestampShort, localtime) << ' ';
		else if (log.at_line_start && log.timestamp_cli == TimestampMode::Long)
			std::cout << std::format(TimestampLong, localtime) << ' ';
		std::cout << s;
	}

	log.at_line_start = s.back() == '\n';
}

/// <summary>
/// 
/// </summary>
/// <typeparam name="...Args"></typeparam>
/// <param name="log"></param>
/// <param name="fmt"></param>
/// <param name="...args"></param>
template <typename ...Args>
void PrintLine(Log& log, std::string_view fmt, const Args&... args)
{
	std::lock_guard lock(log.mutex);
	Print(log, fmt, args...);
	Print(log, "\n"sv);
}

} // end namespace log

///////////////////////////////////////////////////////////////////////////////

template <>
struct std::formatter<tdrp::Vector2df> : std::formatter<std::string>
{
	auto format(const tdrp::Vector2df& in, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{:.2f}, {:.2f}", in.x, in.y);
	}
};

template <>
struct std::formatter<tdrp::Rectf> : std::formatter<std::string>
{
	auto format(const tdrp::Rectf& in, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{:.2f}, {:.2f}, {:.2f}, {:.2f}", in.pos.x, in.pos.y, in.size.x, in.size.y);
	}
};
