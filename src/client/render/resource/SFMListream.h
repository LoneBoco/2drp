#pragma once

#include <memory>
#include <istream>

#include "SFML/System/InputStream.hpp"

namespace tdrp::render
{

class SFMListream : public sf::InputStream
{
public:
	SFMListream() = delete;
	~SFMListream() override {}

	explicit SFMListream(std::istream* stream) : m_stream(stream) {}

	sf::Int64 read(void* data, sf::Int64 size) override
	{
		m_stream->read(static_cast<char*>(data), size);
		return m_stream->gcount();
	}

	sf::Int64 seek(sf::Int64 position) override
	{
		m_stream->seekg(position, m_stream->beg);
		return m_stream->gcount();
	}

	sf::Int64 tell() override
	{
		return m_stream->tellg();
	}

	sf::Int64 getSize() override
	{
		auto current_pos = m_stream->tellg();

		m_stream->seekg(0, m_stream->end);
		auto end_pos = static_cast<sf::Int64>(m_stream->tellg());

		m_stream->seekg(current_pos, m_stream->beg);
		return end_pos;
	}

protected:
	std::istream* m_stream;
};

} // end namespace tdrp::render
