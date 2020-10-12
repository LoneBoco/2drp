#include <fstream>

#include "client/network/ServerPacketHandler.h"

#include "client/game/Game.h"
#include "client/network/DownloadManager.h"

#include "engine/network/Packet.h"
#include "engine/network/PacketsServer.h"
#include "engine/filesystem/common.h"

using tdrp::network::ServerPackets;
using tdrp::network::construct;

namespace tdrp::handlers
{

void handle(Game& game, const packet::SError& packet);
void handle(Game& game, const packet::SLoginStatus& packet);
void handle(Game& game, const packet::SServerInfo& packet);
void handle(Game& game, const packet::STransferFile& packet);
void handle(Game& game, const packet::SSwitchScene& packet);
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
		case ServerPackets::SERVERINFO:
			handle(game, construct<packet::SServerInfo>(packet_data, packet_length));
			break;
		case ServerPackets::TRANSFERFILE:
			handle(game, construct<packet::STransferFile>(packet_data, packet_length));
			break;
		case ServerPackets::SWITCHSCENE:
			handle(game, construct<packet::SSwitchScene>(packet_data, packet_length));
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

void handle(Game& game, const packet::SServerInfo& packet)
{
	const auto& uniqueid = packet.uniqueid();
	const auto& package = packet.package();
	const auto& loadingscene = packet.loadingscene();

	game.Server.FileSystem.Bind(filesystem::path("downloads") / uniqueid);

	BabyDI::Injected<tdrp::DownloadManager> downloader;
	downloader->FilesInQueue = true;

	std::thread process_missing_files([packet]()
	{
		BabyDI::Injected<tdrp::DownloadManager> downloader;
		BabyDI::Injected<tdrp::Game> game;

		// Blocks thread.
		game->Server.FileSystem.WaitUntilFilesSearched();

		std::list<std::string> download_list;
		for (size_t i = 0; i < packet.files_size(); ++i)
		{
			const auto& file_entry = packet.files(i);

			const auto& name = file_entry.name();
			const auto& size = file_entry.size();
			const auto& date = file_entry.date();
			const auto& crc32 = file_entry.crc32();

			// Check to see if we have this file.
			bool request_file = true;
			if (game->Server.FileSystem.HasFile(name))
			{
				auto& data = game->Server.FileSystem.GetFileData(name);
				if (data.FileSize == size && data.TimeSinceEpoch == date && data.CRC32 == crc32)
					request_file = false;
			}

			if (request_file)
				download_list.push_back(name);
		}
		if (!download_list.empty())
			downloader->AddToQueue(std::move(download_list));
		else downloader->TryClearProcessingFlag();
	});
	
	// Show loading scene.
	game.State = GameState::LOADING;
}

void handle(Game& game, const packet::STransferFile& packet)
{
	const auto& name = packet.name();
	const auto& date = packet.date();
	const auto& file = packet.file();

	if (game.Server.DefaultDownloadPath.empty())
	{
		std::cout << "[ERROR] Received STransferFile but no default download path set." << std::endl;
		return;
	}

	// Write the file.
	filesystem::path file_location = game.Server.DefaultDownloadPath / name;
	std::ofstream new_file(file_location.string(), std::ios::binary | std::ios::trunc);
	new_file.write(file.c_str(), file.size());
	new_file.close();

	// Set the last write time.
	// TODO: Improve portability with C++20.
	filesystem::last_write_time(file_location, filesystem::file_time_type(filesystem::file_time_type::duration(date)));

	BabyDI::Injected<tdrp::DownloadManager> downloader;
	downloader->InformComplete(name);
}

void handle(Game& game, const packet::SSwitchScene& packet)
{
	const auto& scene = packet.scene();
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
