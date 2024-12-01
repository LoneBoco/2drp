#pragma once

#include <memory>
#include <istream>

#include "SFML/System/InputStream.hpp"

namespace tdrp::loader
{

class SFMListream : public sf::InputStream
{
public:
	SFMListream() = delete;
	~SFMListream() override {}

	explicit SFMListream(std::istream* stream) : m_stream(stream) {}

	std::optional<std::size_t> read(void* data, std::size_t size) override
	{
		m_stream->read(static_cast<char*>(data), size);
		return m_stream->gcount();
	}

	std::optional<std::size_t> seek(std::size_t position) override
	{
		m_stream->clear();
		m_stream->seekg(position);
		return m_stream->tellg();
	}

	std::optional<std::size_t> tell() override
	{
		return m_stream->tellg();
	}

	std::optional<std::size_t> getSize() override
	{
		auto current_pos = m_stream->tellg();

		m_stream->seekg(0, m_stream->end);
		auto end_pos = static_cast<std::size_t>(m_stream->tellg());

		m_stream->seekg(current_pos);
		return end_pos;
	}

protected:
	std::istream* m_stream;
};

} // end namespace tdrp::loader
