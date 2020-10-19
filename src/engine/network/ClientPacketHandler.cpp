#include "engine/network/ClientPacketHandler.h"

#include "engine/server/Server.h"
#include "engine/server/Player.h"

#include "engine/network/Packet.h"
#include "engine/network/PacketsClient.h"
#include "engine/network/PacketsServer.h"

#include "engine/filesystem/File.h"

using tdrp::server::Server;
using tdrp::server::Player;
using tdrp::network::ClientPackets;
using tdrp::network::ServerPackets;
using tdrp::network::construct;

namespace tdrp::network::handlers
{

void handle(Server* server, std::shared_ptr<Player> player, const packet::CSceneObjectChange& packet);
void handle(Server* server, std::shared_ptr<Player> player, const packet::CRequestFile& packet);
void handle(Server* server, std::shared_ptr<Player> player, const packet::CSendEvent& packet);

/////////////////////////////

bool network_receive(Server* server, const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	if (!server)
		return false;

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

void handle(Server* server, std::shared_ptr<Player> player, const packet::CSceneObjectChange& packet)
{
}

void handle(Server* server, std::shared_ptr<Player> player, const packet::CRequestFile& packet)
{
	for (size_t i = 0; i < packet.file_size(); ++i)
	{
		filesystem::path file_path = packet.file(i);

		// TODO: Better file sending with a class to send data in proper chunks.

		auto file = server->FileSystem.GetFile(file_path.filename());
		if (file != nullptr)
		{
			tdrp::packet::STransferFile message;
			message.set_type(tdrp::packet::STransferFile_Type_PACKAGE);
			message.set_name(file_path.filename().string());
			message.set_date(static_cast<google::protobuf::int64>(file->ModifiedTime()));
			message.set_file(file->ReadAsString());

			server->GetNetwork().Send(player->GetPlayerId(), network::PACKETID(ServerPackets::TRANSFERFILE), network::Channel::FILE, message);
		}
	}
}

void handle(Server* server, std::shared_ptr<Player> player, const packet::CSendEvent& packet)
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

		// Determine objects within the event and handle script.
		auto& hits = scene->FindObjectsInRangeOf({ x, y }, radius);
		for (auto p : hits)
		{
			// TODO: Handle the event script.
		}

		tdrp::packet::SSendEvent message;
		message.set_sender(sender);
		message.set_name(name);
		message.set_data(data);
		message.set_x(x);
		message.set_y(y);
		message.set_radius(radius);

		server->GetNetwork().SendToScene(scene, { x, y }, PACKETID(ServerPackets::SENDEVENT), tdrp::network::Channel::RELIABLE, message);
	}
}

} // end namespace tdrp::handlers
