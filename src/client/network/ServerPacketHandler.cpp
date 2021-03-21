#include <fstream>

#include "client/network/ServerPacketHandler.h"

#include "client/game/Game.h"
#include "client/render/component/RenderComponent.h"
#include "client/render/component/TileMapRenderComponent.h"
#include "client/render/component/TMXRenderComponent.h"
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
void handle(Game& game, const packet::SSceneObjectControl& packet);

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
		case ServerPackets::SCENEOBJECTCONTROL:
			handle(game, construct<packet::SSceneObjectControl>(packet_data, packet_length));
			break;
	}
}

/////////////////////////////

void handle(Game& game, const packet::SError& packet)
{
	const auto& msg = packet.message();
	std::cout << "[ERROR] " << msg << std::endl;
}

void handle(Game& game, const packet::SLoginStatus& packet)
{
	const auto success = packet.success();
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

	// Try to bind the package directory, if it exists.
	game.Server.FileSystem.Bind(filesystem::path("packages") / package);

	if (!game.Server.IsSinglePlayer())
	{
		// If we are a client, bind our downloads directory.
		if (!game.Server.IsHost())
		{
			game.Server.DefaultDownloadPath = filesystem::path("downloads") / uniqueid;
			game.Server.FileSystem.Bind(game.Server.DefaultDownloadPath);
		}

		auto downloader = BabyDI::Get<tdrp::DownloadManager>();
		downloader->FilesInQueue = true;

		std::thread process_missing_files([packet]()
		{
			auto downloader = BabyDI::Get<tdrp::DownloadManager>();
			auto game = BabyDI::Get<tdrp::Game>();

			// Blocks thread.
			game->Server.FileSystem.WaitUntilFilesSearched();

			std::list<std::string> download_list;
			for (int i = 0; i < packet.files_size(); ++i)
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
					auto data = game->Server.FileSystem.GetFileData(name);
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
	}
	
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

	auto downloader = BabyDI::Get<tdrp::DownloadManager>();
	downloader->InformComplete(name);
}

void handle(Game& game, const packet::SSwitchScene& packet)
{
	const auto& scene_name = packet.scene();

	if (game.Player)
	{
		auto scene = game.Server.GetScene(scene_name);
		game.Player->SwitchScene(scene);
		game.OnSceneSwitch.RunAll<Game>(scene);
	}
}

void handle(Game& game, const packet::SClientScript& packet)
{
	const auto& name = packet.name();
	const auto& script = packet.script();

	game.Server.AddClientScript(name, script);
	game.Script.RunScript(name, script, game);
	game.OnCreated.Run<Game>(name);
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
	const auto pid = packet.id();
	const auto ptype = packet.type();
	const auto& pclass = packet.class_();
	const auto& pscript = packet.script();

	auto class_ = game.Server.GetObjectClass(pclass);
	auto type = static_cast<SceneObjectType>(ptype);

	std::cout << ":: Adding scene object " << pid << std::endl;

	std::shared_ptr<SceneObject> so;

	if (game.Server.IsHost())
	{
		so = game.Server.GetSceneObjectById(pid);
	}
	else
	{
		// Create our new scene object.
		switch (type)
		{
			case SceneObjectType::STATIC:
				so = std::make_shared<StaticSceneObject>(class_, pid);
				break;
			case SceneObjectType::ANIMATED:
				so = std::make_shared<AnimatedSceneObject>(class_, pid);
				break;
			case SceneObjectType::TILEMAP:
				so = std::make_shared<TiledSceneObject>(class_, pid);
				break;
			case SceneObjectType::TMX:
				so = std::make_shared<TMXSceneObject>(class_, pid);
				break;
			default:
				so = std::make_shared<SceneObject>(class_, pid);
				break;
		}

		// Add props and attributes.
		for (int i = 0; i < packet.attributes_size(); ++i)
		{
			const auto& attribute = packet.attributes(i);
			switch (attribute.value_case())
			{
				case packet::SSceneObjectNew_Attribute::kAsInt:
					so->Attributes.AddAttribute(attribute.name(), attribute.as_int(), attribute.id());
					break;
				case packet::SSceneObjectNew_Attribute::kAsUint:
					so->Attributes.AddAttribute(attribute.name(), attribute.as_uint(), attribute.id());
					break;
				case packet::SSceneObjectNew_Attribute::kAsFloat:
					so->Attributes.AddAttribute(attribute.name(), attribute.as_float(), attribute.id());
					break;
				case packet::SSceneObjectNew_Attribute::kAsDouble:
					so->Attributes.AddAttribute(attribute.name(), attribute.as_double(), attribute.id());
					break;
				case packet::SSceneObjectNew_Attribute::kAsString:
					so->Attributes.AddAttribute(attribute.name(), attribute.as_string(), attribute.id());
					break;
			}
		}

		for (int i = 0; i < packet.properties_size(); ++i)
		{
			const auto& prop = packet.properties(i);
			auto soprop = so->Properties.Get(prop.name());
			if (soprop)
			{
				switch (prop.value_case())
				{
					case packet::SSceneObjectNew_Attribute::kAsInt:
						soprop->Set(prop.as_int());
						break;
					case packet::SSceneObjectNew_Attribute::kAsUint:
						soprop->Set(prop.as_uint());
						break;
					case packet::SSceneObjectNew_Attribute::kAsFloat:
						soprop->Set(prop.as_float());
						break;
					case packet::SSceneObjectNew_Attribute::kAsDouble:
						soprop->Set(prop.as_double());
						break;
					case packet::SSceneObjectNew_Attribute::kAsString:
						soprop->Set(prop.as_string());
						break;
				}
			}
		}

		// Add it to the current scene.
		if (auto scene = game.Player->GetCurrentScene().lock())
		{
			scene->AddObject(so);
		}
	}

	// Add the render component.
	if (so)
	{
		switch (so->GetType())
		{
			default:
			case SceneObjectType::STATIC:
				so->AddComponent<render::component::RenderComponent>();
				break;
			case SceneObjectType::TILEMAP:
				so->AddComponent<render::component::TileMapRenderComponent>();
				break;
			case SceneObjectType::TMX:
				so->AddComponent<render::component::TMXRenderComponent>();
				break;
			case SceneObjectType::ANIMATED:
				throw "Animated not implemented";
		}
	}

	// Handle the script.
	if (so)
	{
		if (class_)
		{
			game.Server.Script.RunScript("sceneobject_" + std::to_string(pid) + "_class_" + pclass, class_->ScriptClient, so);
		}

		so->ClientScript = pscript;
		game.Server.Script.RunScript("sceneobject" + std::to_string(pid), so->ClientScript, so);
	}
}

void handle(Game& game, const packet::SSceneObjectChange& packet)
{
	const auto id = packet.id();

	auto so = game.Server.GetSceneObjectById(id);
	if (!so)
		return;

	for (int i = 0; i < packet.attributes_size(); ++i)
	{
		const auto& attribute = packet.attributes(i);
		const auto attribute_id = static_cast<uint16_t>(attribute.id());
		auto soattrib = so->Attributes.Get(attribute_id);
		if (!soattrib)
			continue;

		switch (attribute.value_case())
		{
			case packet::SSceneObjectNew_Attribute::kAsInt:
				soattrib->Set(attribute.as_int());
				break;
			case packet::SSceneObjectNew_Attribute::kAsUint:
				soattrib->Set(attribute.as_uint());
				break;
			case packet::SSceneObjectNew_Attribute::kAsFloat:
				soattrib->Set(attribute.as_float());
				break;
			case packet::SSceneObjectNew_Attribute::kAsDouble:
				soattrib->Set(attribute.as_double());
				break;
			case packet::SSceneObjectNew_Attribute::kAsString:
				soattrib->Set(attribute.as_string());
				break;
		}
	}

	for (int i = 0; i < packet.properties_size(); ++i)
	{
		const auto& prop = packet.properties(i);
		const auto prop_id = static_cast<uint16_t>(prop.id());
		auto soprop = so->Properties.Get(PropertyById(prop_id));
		if (!soprop)
			continue;

		switch (prop.value_case())
		{
			case packet::SSceneObjectNew_Attribute::kAsInt:
				soprop->Set(prop.as_int());
				break;
			case packet::SSceneObjectNew_Attribute::kAsUint:
				soprop->Set(prop.as_uint());
				break;
			case packet::SSceneObjectNew_Attribute::kAsFloat:
				soprop->Set(prop.as_float());
				break;
			case packet::SSceneObjectNew_Attribute::kAsDouble:
				soprop->Set(prop.as_double());
				break;
			case packet::SSceneObjectNew_Attribute::kAsString:
				soprop->Set(prop.as_string());
				break;
		}
	}
}

void handle(Game& game, const packet::SSceneObjectDelete& packet)
{
	const auto id = packet.id();
	game.Server.DeleteSceneObject(id);
}

void handle(Game& game, const packet::SSceneObjectControl& packet)
{
	const auto old_id = packet.old_id();
	const auto new_id = packet.new_id();

	auto old_so = game.Server.GetSceneObjectById(old_id);
	auto new_so = game.Server.GetSceneObjectById(new_id);

	game.OnControlledActorChange.RunAll<Game>(old_so, new_so);
	new_so->OnPlayerGainedControl.RunAll<SceneObject>(game.Player);
}

} // end namespace tdrp::handlers
