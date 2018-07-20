#include "client/network/PacketHandler.h"

#include "client/game/Game.h"

#include "engine/network/PacketID.h"
#include "engine/network/PacketsServer.h"

namespace tdrp::handlers
{

void network_receive(Game& game, const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	// Grab our packet id.
	ServerPackets packet = static_cast<ServerPackets>(packet_id);
	switch (packet)
	{
		case ServerPackets::ERROR:
			handle(game, construct<packet::SError>(packet_data, packet_length));
			break;
	}
}

/////////////////////////////

template <class T>
const T construct(const uint8_t* const packet_data, const size_t packet_length)
{
	T packet;
	packet.ParseFromArray(packet_data, packet_length);
	return packet;
}

/////////////////////////////

void handle(Game& game, const packet::SError& packet)
{
	std::string msg = packet.message();
}

} // end namespace tdrp::handlers
