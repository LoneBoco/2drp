#pragma once

#include <fstream>

#include "engine/common.h"
#include "engine/filesystem/common.h"

namespace tdrp::fs
{

class File
{
public:
	File() = delete;
	File(const filesystem::path& file);
	File(const filesystem::path& file, std::unique_ptr<std::ifstream>&& stream);
	~File();

	File(const File& other) : File(other.m_file)
	{
		SetReadPosition(other.GetReadPosition());
	}

	File(File&& other) : m_file(std::move(other.m_file))
	{
		m_stream.swap(other.m_stream);
	}

	File& operator=(const File& other)
	{
		m_file = other.m_file;
		openStream();
	}

	File& operator=(File&& other)
	{
		std::swap(m_file, other.m_file);
		std::swap(m_stream, other.m_stream);
	}

	//! Converts directly into an ifstream.
	operator std::ifstream&() const
	{
		return *(m_stream.get());
	}

	//! Returns if this is a valid file.
	operator bool() const
	{
		return m_stream->is_open();
	}

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
	void openStream();

	filesystem::path m_file;
	mutable std::unique_ptr<std::ifstream> m_stream;
};

} // end namespace tdrp::fs
