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

void handle_ready(Server* server, std::shared_ptr<Player> player);
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
	case ClientPackets::READY:
		handle_ready(server, player);
		return true;
	case ClientPackets::SCENEOBJECTCHANGE:
		handle(server, player, construct<packet::CSceneObjectChange>(packet_data, packet_length));
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
	auto add_attribute = [](packet::SSceneObjectNew& packet, const std::pair<uint16_t, std::shared_ptr<Attribute>>& attribute)
	{
		if (attribute.second->GetType() == AttributeType::INVALID)
			return;

		auto n = packet.add_properties();
		n->set_id(attribute.first);
		n->set_name(attribute.second->GetName());

		switch (attribute.second->GetType())
		{
			case AttributeType::SIGNED:
				n->set_as_int(attribute.second->GetSigned());
				break;
			case AttributeType::UNSIGNED:
				n->set_as_uint(attribute.second->GetUnsigned());
				break;
			case AttributeType::FLOAT:
				n->set_as_float(attribute.second->GetFloat());
				break;
			case AttributeType::DOUBLE:
				n->set_as_double(attribute.second->GetDouble());
				break;
			case AttributeType::STRING:
				n->set_as_string(attribute.second->GetString());
				break;
			default:
				n->set_as_string("Unknown Attribute Error");
				break;
		}
	};

	// Tell the player which scene they are on.
	packet::SSwitchScene switchscene;
	switchscene.set_scene(server->GetPackage()->GetStartingScene());
	server->GetNetwork().Send(player->GetPlayerId(), network::PACKETID(ServerPackets::SWITCHSCENE), network::Channel::RELIABLE, switchscene);

	// Send scene objects to the player.
	// TODO: Maintain a list in memory of their known scene objects.
	// TODO: Use the actual starting position of the player.
	auto scene = server->GetScene(server->GetPackage()->GetStartingScene());
	auto sceneobjects = scene->FindObjectsInRangeOf({ 0.0f, 0.0f }, scene->GetTransmissionDistance());
	for (const auto& sceneobject : sceneobjects)
	{
		packet::SSceneObjectNew object;
		object.set_id(sceneobject->ID);
		object.set_type(static_cast<google::protobuf::uint32>(sceneobject->GetType()));
		object.set_class_(sceneobject->GetClass()->GetName());
		//object.set_script()

		for (const auto& prop : sceneobject->Properties)
		{
			add_attribute(object, prop);
		}

		for (const auto& attr : sceneobject->Attributes)
		{
			add_attribute(object, attr);
		}

		server->GetNetwork().Send(player->GetPlayerId(), network::PACKETID(ServerPackets::SCENEOBJECTNEW), network::Channel::RELIABLE, object);
	}

	// Call the OnPlayerJoin script function.
	server->OnPlayerJoin.RunAll(*server, player);
}

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
			// Handle the event script.
			auto so = server->GetSceneObjectById(sender);
			p->OnEvent.RunAll(*p, so, name, data);
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
