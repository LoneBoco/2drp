#pragma once

#include <fstream>

#include "engine/common.h"
#include "engine/filesystem/common.h"

#include <ZipArchive.h>

namespace tdrp::fs
{

class File
{
public:
	File() = delete;
	File(const filesystem::path& file, std::unique_ptr<std::ifstream>&& stream) : m_file(file), m_stream(std::move(stream)) {};

	File(const filesystem::path& file) : m_file(file)
	{
		Open();
	};

	virtual ~File()
	{
		Close();
	}

	File(const File& other) = delete;
	File& operator=(const File& other) = delete;

	File(File&& other) noexcept : m_file(std::move(other.m_file))
	{
		m_stream.swap(other.m_stream);
	}

	virtual File& operator=(File&& other) noexcept
	{
		std::swap(m_file, other.m_file);
		std::swap(m_stream, other.m_stream);
		return *this;
	}

	bool operator==(const File& other) = delete;

public:
	//! Converts directly into an istream.
	virtual operator std::istream& () const
	{
		return *(m_stream.get());
	}

	//! Converts directly into a shared pointer to the istream.
	virtual operator std::istream* () const
	{
		return m_stream.get();
	}

	//! Returns if this is a valid file.
	virtual operator bool() const
	{
		return Opened();
	}

	//! Opens the file.
	//! \return If the file was successfully opened.
	virtual bool Open() const;

	//! Closes the file.
	virtual void Close() const;

	//! Reads the full file.
	//! \return The file contents.
	virtual std::vector<char> Read() const;

	//! Reads part of a file.
	//! \param count How many bytes to read.
	//! \return The file contents.
	virtual std::vector<char> Read(std::size_t count) const;

	//! Reads from the file until it encounters the token.
	virtual std::vector<char> ReadUntil(const std::string& token) const;
	virtual std::vector<char> ReadUntil(const std::vector<char>& token) const;

	//! Reads the file into a string.
	//! \return The file contents.
	virtual std::string ReadAsString() const;

	//! Reads a line from the file.
	//! \return A string containing a single line, excluding the line ending.
	virtual std::string ReadLine() const;

	//! Reads data into a buffer.
	//! \param buffer The buffer to fill.
	//! \param count How many bytes to read.
	//! \return How much bytes were actually read.
	virtual size_t ReadIntoBuffer(uint8_t* buffer, size_t count);

	//! Reads the position indicator of the file.
	//! \return The position indicator.
	virtual std::streampos GetReadPosition() const;

	//! Sets the position indicator of the file.
	//! \param position The position in the file.
	virtual void SetReadPosition(const std::streampos& position);

	//! Sets the position indicator of the file.
	//! \param offset The offset for our new read position.
	//! \param origin Where we calculate the offset from.
	virtual void SetReadPosition(const std::streamoff& offset, const std::ios_base::seekdir origin = std::ios_base::beg);

	//! Tells us if the file is opened.
	//! \return If the file is opened or not.
	virtual bool Opened() const;

	//! Tells us if we finished reading the file.
	//! \return If we finished reading the file or not.
	virtual bool Finished() const;

	//! Gets the file size.
	//! \return The file size.
	virtual uintmax_t Size() const
	{
		return filesystem::file_size(m_file);
	}

	/// <summary>
	/// Gets the path to the file.
	/// </summary>
	/// <returns>The path to the file.</returns>
	filesystem::path FilePath() const
	{
		return m_file;
	}

	//! Gets the file modified time.
	//! \return The file modified time.
	virtual intmax_t ModifiedTime() const
	{
		return filesystem::last_write_time(m_file).time_since_epoch().count();
	}

	//! Gets the CRC32 of the file.
	//! \return The CRC32 of the file.
	virtual uint32_t Crc32() const;

protected:
	filesystem::path m_file;
	mutable std::unique_ptr<std::istream> m_stream;
};

using FilePtr = std::shared_ptr<File>;


class FileZip : public File
{
public:
	FileZip() = delete;
	FileZip(const filesystem::path& file, ZipArchive::Ptr& archive);
	FileZip(const filesystem::path& file, ZipArchiveEntry::Ptr& zipfile);

	FileZip(const FileZip& other) = delete;
	FileZip& operator=(const FileZip& other) = delete;

	FileZip(FileZip&& other) noexcept : File(std::move(other)), m_zipfile(std::move(other.m_zipfile)) {}
	FileZip& operator=(FileZip&& other) noexcept
	{
		std::swap(m_file, other.m_file);
		std::swap(m_stream, other.m_stream);
		std::swap(m_zipfile, other.m_zipfile);
		return *this;
	}

	//! Opens the file.
	//! \return If the file was successfully opened.
	bool Open() const override;

	//! Closes the file.
	void Close() const override;

	//! Gets the file size.
	//! \return The file size.
	uintmax_t Size() const override
	{
		return m_zipfile->GetSize();
	}

	//! Gets the file modified time.
	//! \return The file modified time.
	intmax_t ModifiedTime() const override
	{
		auto t = std::chrono::system_clock::from_time_t(m_zipfile->GetLastWriteTime());
		return t.time_since_epoch().count();
	}

	uint32_t Crc32() const override
	{
		return m_zipfile->GetCrc32();
	}

protected:
	mutable ZipArchiveEntry::Ptr m_zipfile;
};

uint32_t calculateCRC32(std::istream& stream);
uint32_t calculateCRC32(const filesystem::path& file);

} // end namespace tdrp::fs
