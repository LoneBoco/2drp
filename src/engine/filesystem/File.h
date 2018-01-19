#pragma once

#ifdef __GNUC__
#include <experimental/filesystem>
#elif _MSC_VER
#include <filesystem>
#endif

#include <fstream>

#include "engine/common.h"

#if defined(_MSC_VER) || defined(__GNUC__)
namespace filesystem = std::experimental::filesystem;
#else
namespace filesystem = std::filesystem;
#endif

namespace tdrp::fs
{

class File
{
public:
	File(const filesystem::path& file);
	File(const filesystem::path& file, std::unique_ptr<std::ifstream>&& stream);
	~File();

	//! Reads the full file.
	//! \return The file contents.
	std::vector<char> Read() const;

	//! Reads part of a file.
	//! \param count How many bytes to read.
	//! \return The file contents.
	std::vector<char> Read(std::size_t count) const;

	//! Reads from the file until it encounters the token.
	std::vector<char> ReadUntil(const std::string& token) const;
	std::vector<char> ReadUntil(const std::vector<char>& token) const;

	//! Reads the file into a string.
	//! \return The file contents.
	std::string ReadAsString() const;

	//! Reads a line from the file.
	//! \return A string containing a single line, excluding the line ending.
	std::string ReadLine() const;

	//! Reads the position indicator of the file.
	//! \return The position indicator.
	std::streampos GetReadPosition() const;

	//! Sets the position indicator of the file.
	//! \param position The position in the file.
	void SetReadPosition(const std::streampos& position);

	//! Tells us if we finished reading the file.
	//! \return If we finished reading the file or not.
	inline bool Finished() const
	{
		if (m_stream == nullptr || !m_stream->is_open())
			return true;
		return m_stream->eof();
	}

protected:
	filesystem::path m_file;
	mutable std::unique_ptr<std::ifstream> m_stream;
};

} // end namespace tdrp::fs
