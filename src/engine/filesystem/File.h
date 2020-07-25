#pragma once

#include <fstream>

#include <ZipArchive.h>

#include "engine/common.h"
#include "engine/filesystem/common.h"

namespace tdrp::fs
{

class File
{
public:
	File() = delete;
	File(const filesystem::path& file) : m_file(file) {};
	File(const filesystem::path& file, std::unique_ptr<std::ifstream>&& stream) : m_file(file), m_stream(std::move(stream)) {};
	virtual ~File() {}

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

	//! Returns if this is a valid file.
	virtual operator bool() const
	{
		return Opened();
	}

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

	//! Reads the position indicator of the file.
	//! \return The position indicator.
	virtual std::streampos GetReadPosition() const;

	//! Sets the position indicator of the file.
	//! \param position The position in the file.
	virtual void SetReadPosition(const std::streampos& position);

	//! Tells us if the file is opened.
	//! \return If the file is opened or not.
	virtual bool Opened() const
	{
		return true;
	}

	//! Tells us if we finished reading the file.
	//! \return If we finished reading the file or not.
	virtual bool Finished() const
	{
		return m_stream->eof();
	}

protected:
	filesystem::path m_file;
	mutable std::unique_ptr<std::istream> m_stream;
};


class FileDisk : public File
{
public:
	FileDisk() = delete;
	FileDisk(const filesystem::path& file)
		: File(file)
	{
		openStream();
	}

	~FileDisk() override
	{
		dynamic_cast<std::ifstream&>(*m_stream).close();
	}

	//! Tells us if the file is opened.
	//! \return If the file is opened or not.
	bool Opened() const override;

	//! Tells us if we finished reading the file.
	//! \return If we finished reading the file or not.
	bool Finished() const override;

protected:
	void openStream();
};


class FileZip : public File
{
public:
	FileZip() = delete;
	FileZip(const filesystem::path& file, ZipArchive::Ptr& archive);
	FileZip(const filesystem::path& file, ZipArchiveEntry::Ptr& zipfile);
	virtual ~FileZip();

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

protected:
	mutable ZipArchiveEntry::Ptr m_zipfile;
};

} // end namespace tdrp::fs
