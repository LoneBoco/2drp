#include "engine/network/ClientPacketHandler.h"

#include "engine/server/Server.h"
#include "engine/server/Player.h"

#include "engine/network/Packet.h"
#include "engine/network/PacketsClient.h"
#include "engine/network/PacketsServer.h"

using tdrp::server::Server;
using tdrp::server::Player;
using tdrp::network::ClientPackets;
using tdrp::network::ServerPackets;
using tdrp::network::construct;

namespace tdrp::network::handlers
{

void handle(std::shared_ptr<Server> server, std::shared_ptr<Player> player, const packet::CSceneObjectChange& packet);
void handle(std::shared_ptr<Server> server, std::shared_ptr<Player> player, const packet::CSendEvent& packet);

/////////////////////////////

bool network_receive(std::shared_ptr<Server> server, const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	// Get our player.
	std::shared_ptr<Player> player = server->GetPlayerById(id);

	// Grab our packet id.
	ClientPackets packet = static_cast<ClientPackets>(packet_id);
	switch (packet)
	{
	case ClientPackets::SCENEOBJECTCHANGE:
		handle(server, player, construct<packet::CSceneObjectChange>(packet_data, packet_length));
		return true;
	case ClientPackets::SENDEVENT:
		handle(server, player, construct<packet::CSendEvent>(packet_data, packet_length));
		return true;
	}

	return false;
}

/////////////////////////////

void handle(std::shared_ptr<Server> server, std::shared_ptr<Player> player, const packet::CSceneObjectChange& packet)
{
}

void handle(std::shared_ptr<Server> server, std::shared_ptr<Player> player, const packet::CSendEvent& packet)
{
	if (server == nullptr)
		return;

	std::shared_ptr<tdrp::scene::Scene> scene = player->GetCurrentScene().lock();
	if (scene)
	{
		const auto& sender = packet.sender();
		const auto& name = packet.name();
		const auto& data = packet.data();
		const auto& x = packet.x();
		const auto& y = packet.y();
		const auto& radius = packet.radius();

		// TODO: Determine objects within the event and handle script.

		tdrp::packet::SSendEvent message;
		message.set_sender(sender);
		message.set_name(name);
		message.set_data(data);
		message.set_x(x);
		message.set_y(y);
		message.set_radius(radius);

		server->Network.SendToScene(scene, { x, y }, PACKETID(ServerPackets::SENDEVENT), tdrp::network::Channel::RELIABLE, message);
	}
}

} // end namespace tdrp::handlers
