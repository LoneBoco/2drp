#pragma once

#include "engine/common.h"

namespace tdrp::network
{

template <class T>
class Packet
{
public:
	Packet() = default;
	~Packet() = default;

	Packet(const Packet& other) = delete;
	Packet(Packet&& other) = delete;
	Packet& operator=(const Packet& other) = delete;
	Packet& operator=(Packet&& other) = delete;

	//std::vector<uint8_t> GetBytes()

private:
	T m_packet;
};

} // end namespace tdrp::network
