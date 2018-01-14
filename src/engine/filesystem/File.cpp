#include "engine/filesystem/File.h"

#include <sstream>

namespace tdrp
{
namespace fs
{

/////////////////////////////

File::File(const filesystem::path& file)
	: m_file(file)
{
	m_stream = std::make_shared<std::ifstream>(file, std::ios::binary);
}

File::File(const filesystem::path& file, ZipArchiveEntry::Ptr& zip )
	: m_file(file), m_zip(zip)
{
	m_stream = zip->GetDecompressionStream();
}

File::~File()
{
	//m_stream->close();
}

std::vector<char> File::Read() const
{
	if (Finished())
		return std::vector<char>();

	// Seek to the end and get the file size.
	m_stream->seekg(0, std::ios::end);
	auto size = m_stream->tellg();

	// Seek to the start and read into the vector.
	std::vector<char> result(size);
	m_stream->seekg(0);
	m_stream->read(result.data(), size);
	return result;
}

std::vector<char> File::Read(std::size_t count) const
{
	if (Finished())
		return std::vector<char>();

	std::vector<char> result(count);
	auto read = m_stream->readsome(result.data(), count);
	result.resize(read);
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
	if (Finished())
		return std::string();

	std::stringstream s;
	s << m_stream->rdbuf();
	return s.str();
}

std::string File::ReadLine() const
{
	if (Finished())
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
	m_stream->seekg(position);
}

} // end namespace fs
} // end namespace tdrp
