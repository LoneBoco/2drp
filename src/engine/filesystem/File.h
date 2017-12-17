#pragma once

#include <filesystem>
#include <fstream>

#include <ZipArchiveEntry.h>

#include "engine/common.h"

#ifdef _MSC_VER
namespace filesystem = std::experimental::filesystem;
#else
namespace filesystem = std::filesystem;
#endif

namespace tdrp
{
namespace fs
{

class File
{
public:
	File(const filesystem::path& file);
	File(const filesystem::path& file, ZipArchiveEntry::Ptr& zip);
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
		if (m_stream == nullptr)
			return true;
		return m_stream->eof();
	}

protected:
	filesystem::path m_file;
	ZipArchiveEntry::Ptr m_zip;
	mutable std::shared_ptr<std::istream> m_stream;
};

} // end namespace fs
} // end namespace tdrp
