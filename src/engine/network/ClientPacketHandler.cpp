#include "engine/network/ClientPacketHandler.h"

#include "engine/server/Server.h"
#include "engine/server/Player.h"

#include "engine/network/Packet.h"
#include "engine/network/PacketsClient.h"
#include "engine/network/PacketsServer.h"
#include "engine/network/construct/SceneObject.h"

#include "engine/filesystem/File.h"

using tdrp::server::Server;
using tdrp::server::Player;
using tdrp::network::ClientPackets;
using tdrp::network::ServerPackets;
using tdrp::network::construct;

namespace tdrp::network::handlers
{

void handle_ready(Server* server, std::shared_ptr<Player> player);
void handle(Server* server, std::shared_ptr<Player> player, const packet::CSceneObjectChange& packet);
void handle(Server* server, std::shared_ptr<Player> player, const packet::CSceneObjectUnfollow& packet);
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
	case ClientPackets::READY:
		handle_ready(server, player);
		return true;
	case ClientPackets::SCENEOBJECTCHANGE:
		handle(server, player, construct<packet::CSceneObjectChange>(packet_data, packet_length));
		return true;
	case ClientPackets::SCENEOBJECTUNFOLLOW:
		handle(server, player, construct<packet::CSceneObjectUnfollow>(packet_data, packet_length));
		return true;
	case ClientPackets::REQUESTFILE:
		handle(server, player, construct<packet::CRequestFile>(packet_data, packet_length));
		return true;
	case ClientPackets::SENDEVENT:
		handle(server, player, construct<packet::CSendEvent>(packet_data, packet_length));
		return true;
	}

	return false;
}

/////////////////////////////

void handle_ready(Server* server, std::shared_ptr<Player> player)
{
	// Switch to the starting scene.
	auto scene = server->GetScene(server->GetPackage()->GetStartingScene());
	player->SwitchScene(scene);

	// Call the OnPlayerJoin script function.
	server->OnPlayerJoin.RunAll<Server>(player);
}

void handle(Server* server, std::shared_ptr<Player> player, const packet::CSceneObjectChange& packet)
{
	const auto id = packet.id();

	auto so = server->GetSceneObjectById(id);
	if (!so)
		return;

	// Check if we control this scene object before allowing updates.
	if (player->GetCurrentControlledSceneObject().lock() != so)
		return;

	// Load the changed attributes.
	for (int i = 0; i < packet.attributes_size(); ++i)
	{
		const auto& attribute = packet.attributes(i);
		const auto attribute_id = static_cast<uint16_t>(attribute.id());
		auto soattrib = so->Attributes.Get(attribute_id);
		if (!soattrib)
			continue;

		switch (attribute.value_case())
		{
			case packet::CSceneObjectChange_Attribute::kAsInt:
				soattrib->Set(attribute.as_int());
				break;
			case packet::CSceneObjectChange_Attribute::kAsUint:
				soattrib->Set(attribute.as_uint());
				break;
			case packet::CSceneObjectChange_Attribute::kAsFloat:
				soattrib->Set(attribute.as_float());
				break;
			case packet::CSceneObjectChange_Attribute::kAsDouble:
				soattrib->Set(attribute.as_double());
				break;
			case packet::CSceneObjectChange_Attribute::kAsString:
				soattrib->Set(attribute.as_string());
				break;
			}
	}

	// Load the changed properties.
	for (int i = 0; i < packet.properties_size(); ++i)
	{
		const auto& prop = packet.properties(i);
		const auto prop_id = static_cast<uint16_t>(prop.id());
		auto soprop = so->Properties.Get(PropertyById(prop_id));
		if (!soprop)
			continue;

		switch (prop.value_case())
		{
			case packet::CSceneObjectChange_Attribute::kAsInt:
				soprop->Set(prop.as_int());
				break;
			case packet::CSceneObjectChange_Attribute::kAsUint:
				soprop->Set(prop.as_uint());
				break;
			case packet::CSceneObjectChange_Attribute::kAsFloat:
				soprop->Set(prop.as_float());
				break;
			case packet::CSceneObjectChange_Attribute::kAsDouble:
				soprop->Set(prop.as_double());
				break;
			case packet::CSceneObjectChange_Attribute::kAsString:
				soprop->Set(prop.as_string());
				break;
		}
	}

	if (!server->IsHost())
	{
		so->Attributes.ClearDirty();
		so->Properties.ClearDirty();
	}
}

void handle(Server* server, std::shared_ptr<Player> player, const packet::CSceneObjectUnfollow& packet)
{
	for (int i = 0; i < packet.id_size(); ++i)
	{
		const auto& id = packet.id(i);
		player->FollowedSceneObjects.erase(id);

		std::cout << "Removing scene object " << id << " from player " << player->GetPlayerId() << "." << std::endl;
	}
}

void handle(Server* server, std::shared_ptr<Player> player, const packet::CRequestFile& packet)
{
	for (int i = 0; i < packet.file_size(); ++i)
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

		Vector2df position{ x, y };

		// Determine objects within the event and handle script.
		auto hits = scene->FindObjectsInRangeOf(position, radius);
		for (auto p : hits)
		{
			// Handle the event script.
			auto so = server->GetSceneObjectById(sender);
			p->OnEvent.RunAll<SceneObject>(so, name, data, position, radius);
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
