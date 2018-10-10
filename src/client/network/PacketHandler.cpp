#include "client/network/PacketHandler.h"

#include "client/game/Game.h"

#include "engine/network/PacketID.h"
#include "engine/network/PacketsServer.h"

namespace tdrp::handlers
{

void handle(Game& game, const packet::SError& packet);
void handle(Game& game, const packet::SLoginStatus& packet);
void handle(Game& game, const packet::SClientScript& packet);
void handle(Game& game, const packet::SClientScriptDelete& packet);
void handle(Game& game, const packet::SClass& packet);
void handle(Game& game, const packet::SClassDelete& packet);
void handle(Game& game, const packet::SSceneObjectNew& packet);
void handle(Game& game, const packet::SSceneObjectChange& packet);
void handle(Game& game, const packet::SSceneObjectDelete& packet);

/////////////////////////////

void network_receive(Game& game, const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	// Grab our packet id.
	ServerPackets packet = static_cast<ServerPackets>(packet_id);
	switch (packet)
	{
		case ServerPackets::ERROR:
			handle(game, construct<packet::SError>(packet_data, packet_length));
			break;
		case ServerPackets::LOGINSTATUS:
			handle(game, construct<packet::SLoginStatus>(packet_data, packet_length));
			break;
		case ServerPackets::CLIENTSCRIPT:
			handle(game, construct<packet::SClientScript>(packet_data, packet_length));
			break;
		case ServerPackets::CLIENTSCRIPTDELETE:
			handle(game, construct<packet::SClientScriptDelete>(packet_data, packet_length));
			break;
		case ServerPackets::CLASS:
			handle(game, construct<packet::SClass>(packet_data, packet_length));
			break;
		case ServerPackets::CLASSDELETE:
			handle(game, construct<packet::SClassDelete>(packet_data, packet_length));
			break;
		case ServerPackets::SCENEOBJECTNEW:
			handle(game, construct<packet::SSceneObjectNew>(packet_data, packet_length));
			break;
		case ServerPackets::SCENEOBJECTCHANGE:
			handle(game, construct<packet::SSceneObjectChange>(packet_data, packet_length));
			break;
		case ServerPackets::SCENEOBJECTDELETE:
			handle(game, construct<packet::SSceneObjectDelete>(packet_data, packet_length));
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
	const auto& msg = packet.message();
}

void handle(Game& game, const packet::SLoginStatus& packet)
{
	const auto& success = packet.success();
	const auto& msg = packet.message();

	if (success)
	{
		std::cout << "<- Login successful." << std::endl;
	}
	else
	{
		std::cout << "<- Login failed: " << msg << std::endl;
	}
}

void handle(Game& game, const packet::SClientScript& packet)
{
	const auto& name = packet.name();
	const auto& script = packet.script();

	game.Server.AddClientScript(name, script);
}

void handle(Game& game, const packet::SClientScriptDelete& packet)
{
	const auto& name = packet.name();

	game.Server.DeleteClientScript(name);
}

void handle(Game& game, const packet::SClass& packet)
{
	const auto& name = packet.name();
	const auto& script_client = packet.scriptclient();
	const auto& script_server = packet.scriptserver();

	auto c = game.Server.GetObjectClass(name);
	c->ScriptClient = script_client;
	c->ScriptServer = script_server;
	c->Attributes.GetMap().clear();
	
	for (int i = 0; i < packet.attributes_size(); ++i)
	{
		const auto& attr = packet.attributes(i);
		const auto& id = attr.id();
		const auto& name = attr.name();
		switch (attr.value_case())
		{
			case packet::SClass_Attribute::ValueCase::kAsInt:
				c->Attributes.AddAttribute(name, attr.as_int(), static_cast<uint16_t>(id));
				break;
			case packet::SClass_Attribute::ValueCase::kAsUint:
				c->Attributes.AddAttribute(name, attr.as_uint(), static_cast<uint16_t>(id));
				break;
			case packet::SClass_Attribute::ValueCase::kAsFloat:
				c->Attributes.AddAttribute(name, attr.as_float(), static_cast<uint16_t>(id));
				break;
			case packet::SClass_Attribute::ValueCase::kAsDouble:
				c->Attributes.AddAttribute(name, attr.as_double(), static_cast<uint16_t>(id));
				break;
			case packet::SClass_Attribute::ValueCase::kAsString:
				c->Attributes.AddAttribute(name, attr.as_string(), static_cast<uint16_t>(id));
				break;
		}
	}
}

void handle(Game& game, const packet::SClassDelete& packet)
{
	const auto& name = packet.name();
	game.Server.DeleteObjectClass(name);
}

void handle(Game& game, const packet::SSceneObjectNew& packet)
{
}

void handle(Game& game, const packet::SSceneObjectChange& packet)
{
}

void handle(Game& game, const packet::SSceneObjectDelete& packet)
{
	const auto& id = packet.id();
}

} // end namespace tdrp::handlers
