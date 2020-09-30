#include <sstream>

#include <boost/crc.hpp>

#include "engine/filesystem/File.h"

namespace tdrp::fs
{

/////////////////////////////

bool File::Open() const
{
	if (m_stream)
		dynamic_cast<std::ifstream&>(*m_stream).close();

	m_stream = std::make_unique<std::ifstream>(m_file, std::ios::binary);
	return true;
}

void File::Close() const
{
	dynamic_cast<std::ifstream&>(*m_stream).close();
}

std::vector<char> File::Read() const
{
	if (!Opened() || Finished())
		return std::vector<char>();

	// Seek to the end and get the file size.
	m_stream->seekg(0, std::ios::end);
	auto size = m_stream->tellg();

	// Seek to the start and read into the vector.
	std::vector<char> result(static_cast<size_t>(size));
	m_stream->seekg(0);
	m_stream->read(result.data(), size);
	return result;
}

std::vector<char> File::Read(std::size_t count) const
{
	if (!Opened() || Finished())
		return std::vector<char>();

	std::vector<char> result(count);
	auto read = m_stream->readsome(result.data(), count);
	result.resize(static_cast<size_t>(read));
	return result;
}

std::vector<char> File::ReadUntil(const std::string& token) const
{
	return std::vector<char>();
}

std::vector<char> File::ReadUntil(const std::vector<char>& token) const
{
	return std::vector<char>();
}

std::string File::ReadAsString() const
{
	if (!Opened() || Finished())
		return std::string();

	std::stringstream s;
	s << m_stream->rdbuf();
	return s.str();
}

std::string File::ReadLine() const
{
	if (!Opened() || Finished())
		return std::string();

	std::string result;
	std::getline(*m_stream, result);

	return result;
}

std::streampos File::GetReadPosition() const
{
	return m_stream->tellg();
}

void File::SetReadPosition(const std::streampos& position)
{
	if (Opened())
		m_stream->seekg(position);
}

bool File::Opened() const
{
	return dynamic_cast<std::ifstream&>(*m_stream).is_open();
}

bool File::Finished() const
{
	if (m_stream == nullptr || !Opened())
		return true;
	return m_stream->eof();
}

uint32_t File::Crc32() const
{
	std::ifstream stream{ m_file, std::ios::binary };
	return calculateCRC32(stream);
}

/////////////////////////////

FileZip::FileZip(const filesystem::path& file, ZipArchive::Ptr& archive)
	: File(file)
{
	m_zipfile = archive->GetEntry(file.string());
	Open();
}

FileZip::FileZip(const filesystem::path& file, ZipArchiveEntry::Ptr& zipfile)
	: File(file), m_zipfile(zipfile)
{
	Open();
}

bool FileZip::Open() const
{
	if (m_zipfile)
	{
		m_stream = std::unique_ptr<std::istream>(m_zipfile->GetDecompressionStream());
		return true;
	}
	return false;
}

void FileZip::Close() const
{
	if (m_stream)
		m_stream.release();

	if (m_zipfile)
		m_zipfile->CloseDecompressionStream();
}

/////////////////////////////

uint32_t calculateCRC32(std::istream& stream)
{
	// Calculate CRC32 if we can.
	boost::crc_32_type crc32;
	if (stream)
	{
		do
		{
			constexpr size_t READBUFFER_SIZE = 1024 * 8;

			char buffer[READBUFFER_SIZE];
			stream.read(buffer, READBUFFER_SIZE);

			crc32.process_bytes(buffer, stream.gcount());
		} while (stream);
	}
	else return 0;

	return crc32.checksum();
}

uint32_t calculateCRC32(const filesystem::path& file)
{
	std::ifstream ifs{ file, std::ios::binary };
	return calculateCRC32(ifs);
}

} // end namespace tdrp::fs
