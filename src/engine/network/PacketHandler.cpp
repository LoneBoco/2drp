#include <PlayRho/PlayRho.hpp>

#include "engine/common.h"

#include "engine/network/PacketHandler.h"

#include "engine/server/Server.h"
#include "engine/server/Player.h"

#include "engine/network/Packet.h"
#include "engine/network/PacketsInc.h"
#include "engine/network/construct/SceneObject.h"

#include "engine/item/Item.h"
#include "engine/filesystem/File.h"
#include "engine/filesystem/Log.h"


using tdrp::server::Server;
using tdrp::server::Player;
using tdrp::network::Packets;
using tdrp::network::construct;

namespace tdrp::network::handlers
{

void handle_ready(Server& server, const uint16_t playerId);

void handle(Server& server, const uint16_t playerId, const packet::Error& packet);
void handle(Server& server, const uint16_t playerId, const packet::Login& packet);
void handle(Server& server, const uint16_t playerId, const packet::LoginStatus& packet);
void handle(Server& server, const uint16_t playerId, const packet::ServerInfo& packet);
void handle(Server& server, const uint16_t playerId, const packet::FileRequest& packet);
void handle(Server& server, const uint16_t playerId, const packet::FileTransfer& packet);
void handle(Server& server, const uint16_t playerId, const packet::SwitchScene& packet);
void handle(Server& server, const uint16_t playerId, const packet::ClientControlScript& packet);
void handle(Server& server, const uint16_t playerId, const packet::ClientScriptAdd& packet);
void handle(Server& server, const uint16_t playerId, const packet::ClientScriptDelete& packet);
void handle(Server& server, const uint16_t playerId, const packet::ClassAdd& packet);
void handle(Server& server, const uint16_t playerId, const packet::ClassDelete& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneObjectNew& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneObjectChange& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneObjectDelete& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneObjectOwnership& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneObjectControl& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneObjectUnfollow& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneObjectShapes& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneObjectChunkData& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneObjectRequestChunkData& packet);
void handle(Server& server, const uint16_t playerId, const packet::SceneTilesetAdd& packet);
void handle(Server& server, const uint16_t playerId, const packet::SendEvent& packet);
void handle(Server& server, const uint16_t playerId, const packet::FlagSet& packet);
void handle(Server& server, const uint16_t playerId, const packet::ItemDefinition& packet);
void handle(Server& server, const uint16_t playerId, const packet::ItemAdd& packet);
void handle(Server& server, const uint16_t playerId, const packet::ItemDelete& packet);

/////////////////////////////

#define HANDLE(cl) handle(server, playerId, construct< cl >(packet_data, packet_length))

void network_receive_server(Server& server, const uint16_t playerId, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	// Grab our packet id.
	Packets packet = static_cast<Packets>(packet_id);
	switch (packet)
	{
		case Packets::CLIENTREADY:
			handle_ready(server, playerId);
			break;
		case Packets::ERROR:
			HANDLE(packet::Error);
			break;
		case Packets::LOGIN:
			HANDLE(packet::Login);
			break;
		case Packets::LOGINSTATUS:
			HANDLE(packet::LoginStatus);
			break;
		case Packets::SERVERINFO:
			HANDLE(packet::ServerInfo);
			break;
		case Packets::FILEREQUEST:
			HANDLE(packet::FileRequest);
			break;
		case Packets::FILETRANSFER:
			HANDLE(packet::FileTransfer);
			break;
		case Packets::SWITCHSCENE:
			HANDLE(packet::SwitchScene);
			break;
		case Packets::CLIENTCONTROLSCRIPT:
			HANDLE(packet::ClientControlScript);
			break;
		case Packets::CLIENTSCRIPTADD:
			HANDLE(packet::ClientScriptAdd);
			break;
		case Packets::CLIENTSCRIPTDELETE:
			HANDLE(packet::ClientScriptDelete);
			break;
		case Packets::CLASSADD:
			HANDLE(packet::ClassAdd);
			break;
		case Packets::SCENEOBJECTNEW:
			HANDLE(packet::SceneObjectNew);
			break;
		case Packets::SCENEOBJECTCHANGE:
			HANDLE(packet::SceneObjectChange);
			break;
		case Packets::SCENEOBJECTDELETE:
			HANDLE(packet::SceneObjectDelete);
			break;
		case Packets::SCENEOBJECTOWNERSHIP:
			HANDLE(packet::SceneObjectOwnership);
			break;
		case Packets::SCENEOBJECTCONTROL:
			HANDLE(packet::SceneObjectControl);
			break;
		case Packets::SCENEOBJECTUNFOLLOW:
			HANDLE(packet::SceneObjectUnfollow);
			break;
		case Packets::SCENEOBJECTSHAPES:
			HANDLE(packet::SceneObjectShapes);
			break;
		case Packets::SCENEOBJECTCHUNKDATA:
			HANDLE(packet::SceneObjectChunkData);
			break;
		case Packets::SCENEOBJECTREQUESTCHUNKDATA:
			HANDLE(packet::SceneObjectRequestChunkData);
			break;
		case Packets::SCENETILESETADD:
			HANDLE(packet::SceneTilesetAdd);
			break;
		case Packets::SENDEVENT:
			HANDLE(packet::SendEvent);
			break;
		case Packets::FLAGSET:
			HANDLE(packet::FlagSet);
			break;
		case Packets::ITEMDEFINITION:
			HANDLE(packet::ItemDefinition);
			break;
		case Packets::ITEMADD:
			HANDLE(packet::ItemAdd);
			break;
		case Packets::ITEMDELETE:
			HANDLE(packet::ItemDelete);
			break;
	}
}

#undef HANDLE

/////////////////////////////

static AttributePtr load_attribute_from_packet(const packet::Attribute& packet_attribute, ObjectAttributes& container)
{
	const auto attribute_id = static_cast<uint16_t>(packet_attribute.id());
	auto attribute = container.Get(attribute_id);
	if (!attribute)
	{
		const auto& name = packet_attribute.name();
		attribute = container.GetOrCreate(name);
	}
	if (!attribute) return nullptr;

	switch (packet_attribute.value_case())
	{
		case packet::Attribute::kAsInt:
			attribute->Set(packet_attribute.as_int());
			break;
		case packet::Attribute::kAsDouble:
			attribute->Set(packet_attribute.as_double());
			break;
		case packet::Attribute::kAsString:
			attribute->Set(packet_attribute.as_string());
			break;
	}

	attribute->Replicated = packet_attribute.replicated();
	attribute->NetworkUpdateRate = std::chrono::milliseconds{ packet_attribute.update_rate() };
	return attribute;
}

/////////////////////////////

void handle_ready(Server& server, const uint16_t playerId)
{
	log::PrintLine("<- Client {} is ready.", playerId);
	server.ProcessPlayerJoin(playerId);
}

/////////////////////////////

void handle(Server& server, const uint16_t playerId, const packet::Error& packet)
{
	auto& error = packet.message();
	log::PrintLine("Error: {}", error);
}

void handle(Server& server, const uint16_t playerId, const packet::Login& packet)
{
	assert(false);
}

void handle(Server& server, const uint16_t playerId, const packet::LoginStatus& packet)
{
	const auto success = packet.success();
	const auto& msg = packet.message();
	const auto player_id = packet.player_id();

	if (success)
	{
		log::PrintLine("<- Login successful.  Player id: {}", player_id);
		server.SetPlayerNumber(player_id);
	}
	else
	{
		log::PrintLine("<- Login failed: {}", msg);
		// TODO: Disconnect.
	}
}

void handle(Server& server, const uint16_t playerId, const packet::ServerInfo& packet)
{
	const auto& uniqueid = packet.uniqueid();
	const auto& package = packet.package();
	const auto& loadingscene = packet.loadingscene();

	// Try to bind the package directory, if it exists.
	//server.FileSystem.Bind(filesystem::path("packages") / package);

	if (server.IsSinglePlayer())
		return;

	// If we are a client, bind our downloads directory.
	if (!server.IsHost())
	{
		server.DefaultDownloadPath = filesystem::path("downloads") / uniqueid;
		server.FileSystem.Bind(fs::FileCategory::ASSETS, server.DefaultDownloadPath);
	}

	// Rage.
	auto packet_copy = std::make_shared<packet::ServerInfo>();
	packet_copy->CopyFrom(packet);

	std::thread process_missing_files(
		[=, &server]()
		{
			// Blocks thread.
			server.FileSystem.WaitUntilFilesSearched();

			std::list<std::string> download_list;
			for (int i = 0; i < packet_copy->files_size(); ++i)
			{
				const auto& file_entry = packet_copy->files(i);

				const auto& name = file_entry.name();
				const auto size = file_entry.size();
				const auto date = file_entry.date();
				const auto crc32 = file_entry.crc32();

				// Check to see if we have this file.
				bool request_file = true;
				if (server.FileSystem.HasFile(fs::FileCategory::ASSETS, name))
				{
					auto data = server.FileSystem.GetFileData(fs::FileCategory::ASSETS, name);
					if (data.FileSize == size && data.TimeSinceEpoch == date && data.CRC32 == crc32)
						request_file = false;
				}

				if (request_file)
					download_list.push_back(name);
			}
			if (!download_list.empty())
				server.DownloadManager.AddToQueue(std::move(download_list));
			else server.DownloadManager.TryClearProcessingFlag();
		}
	);
	process_missing_files.detach();
}

void handle(Server& server, const uint16_t playerId, const packet::FileRequest& packet)
{
	if (server.IsSinglePlayer())
		return;

	for (int i = 0; i < packet.file_size(); ++i)
	{
		filesystem::path file_path = packet.file(i);

		// TODO: Better file sending with a class to send data in proper chunks.

		auto file = server.FileSystem.GetFile(fs::FileCategory::ASSETS, file_path.filename());
		if (file != nullptr)
		{
			tdrp::packet::FileTransfer message;
			message.set_type(tdrp::packet::FileTransfer_Type_PACKAGE);
			message.set_name(file_path.filename().string());
			message.set_date(static_cast<google::protobuf::int64>(file->ModifiedTime()));
			message.set_file(file->ReadAsString());

			server.GetNetwork().Send(playerId, network::PACKETID(Packets::FILETRANSFER), network::Channel::FILE, message);
		}
	}
}

void handle(Server& server, const uint16_t playerId, const packet::FileTransfer& packet)
{
	if (server.IsSinglePlayer())
		return;

	const auto& name = packet.name();
	const auto date = packet.date();
	const auto& file = packet.file();

	if (server.DefaultDownloadPath.empty())
	{
		log::PrintLine("** [ERROR] Received TransferFile but no default download path set.");
		return;
	}

	// Write the file.
	filesystem::path file_location = server.DefaultDownloadPath / name;
	std::ofstream new_file(file_location.string(), std::ios::binary | std::ios::trunc);
	new_file.write(file.c_str(), file.size());
	new_file.close();

	// Set the last write time.
	// TODO: Improve portability with C++20.
	filesystem::last_write_time(file_location, filesystem::file_time_type(filesystem::file_time_type::duration(date)));

	server.DownloadManager.InformComplete(name);
}

void handle(Server& server, const uint16_t playerId, const packet::SwitchScene& packet)
{
	// Client handles.
	//assert(false);
}

void handle(Server& server, const uint16_t playerId, const packet::ClientControlScript& packet)
{
	// Client handles.
	//assert(false);
}

void handle(Server& server, const uint16_t playerId, const packet::ClientScriptAdd& packet)
{
	// Client handles.
	//assert(false);
}

void handle(Server& server, const uint16_t playerId, const packet::ClientScriptDelete& packet)
{
	// Client handles.
	//assert(false);
}

void handle(Server& server, const uint16_t playerId, const packet::ClassAdd& packet)
{
	const auto& name = packet.name();
	const auto& script_client = packet.scriptclient();
	const auto& script_server = packet.scriptserver();

	log::PrintLine(":: Adding class {}.", name);

	auto c = server.GetObjectClass(name);
	c->ClientScript = script_client;
	c->ServerScript = script_server;
	c->Attributes.GetMap().clear();

	for (int i = 0; i < packet.attributes_size(); ++i)
	{
		const auto& attr = packet.attributes(i);
		const auto id = attr.id();
		const auto& name = attr.name();
		const auto replicated = attr.replicated();
		const auto update_rate = attr.update_rate();

		AttributePtr attribute;
		switch (attr.value_case())
		{
			case packet::Attribute::ValueCase::kAsInt:
				attribute = c->Attributes.AddAttribute(name, attr.as_int(), static_cast<uint16_t>(id));
				break;
			case packet::Attribute::ValueCase::kAsDouble:
				attribute = c->Attributes.AddAttribute(name, attr.as_double(), static_cast<uint16_t>(id));
				break;
			case packet::Attribute::ValueCase::kAsString:
				attribute = c->Attributes.AddAttribute(name, attr.as_string(), static_cast<uint16_t>(id));
				break;
		}

		attribute->Replicated = replicated;
		attribute->NetworkUpdateRate = std::chrono::milliseconds{ update_rate };
	}
}

void handle(Server& server, const uint16_t playerId, const packet::ClassDelete& packet)
{
	const auto& name = packet.name();

	log::PrintLine("!! Deleting class {}", name);

	server.DeleteObjectClass(name);
}

void handle(Server& server, const uint16_t playerId, const packet::SceneObjectNew& packet)
{
	const auto pid = packet.id();
	const auto ptype = packet.type();
	const auto& pclass = packet.class_();
	const auto& pscript = packet.script();
	const auto preplicatechanges = packet.replicatechanges();
	const auto pignoresevents = packet.ignoresevents();
	const auto pattached_to = packet.attached_to();
	const auto& pscene = packet.scene();

	auto type = static_cast<SceneObjectType>(ptype);

	auto scene = server.GetScene(pscene);
	if (!scene)
	{
		log::PrintLine("** ERROR: Attempted to add scene object to scene \"{}\" but the scene does not exist.", pscene);
		return;
	}

	std::shared_ptr<SceneObject> so = server.GetSceneObjectById(pid);
	if (so != nullptr)
	{
		if (!server.IsHost())
			log::PrintLine("!! Scene object {} ({}) already exists, skipping add.", pid, pclass);
		else log::PrintLine("!! Scene object {} ({}) already exists when it shouldn't!", pid, pclass);
	}
	else
	{
		log::PrintLine(":: Adding scene object {} ({}).", pid, pclass);

		// Create the scene object.
		so = server.CreateSceneObject(type, pclass, pid);
		so->ClientScript = pscript;
		so->ReplicateChanges = preplicatechanges;
		so->IgnoresEvents = pignoresevents;

		// Set owner.
		auto powner = packet.owner();
		so->SetOwningPlayer(static_cast<PlayerID>(powner));

		// Set attached to.
		// Do this before we read in attributes/props as attaching sets the position to {0, 0}.
		if (pattached_to != 0)
		{
			auto attached = server.GetSceneObjectById(pattached_to);
			if (attached)
				so->AttachTo(attached);
		}

		// Add attributes.
		for (int i = 0; i < packet.attributes_size(); ++i)
		{
			const auto& attribute = packet.attributes(i);
			AttributePtr attr = nullptr;

			switch (attribute.value_case())
			{
				case packet::Attribute::kAsInt:
					attr = so->Attributes.AddAttribute(attribute.name(), attribute.as_int(), attribute.id());
					break;
				case packet::Attribute::kAsDouble:
					attr = so->Attributes.AddAttribute(attribute.name(), attribute.as_double(), attribute.id());
					break;
				case packet::Attribute::kAsString:
					attr = so->Attributes.AddAttribute(attribute.name(), attribute.as_string(), attribute.id());
					break;
			}

			if (attr)
			{
				attr->Replicated = attribute.replicated();
				attr->NetworkUpdateRate = std::chrono::milliseconds{ attribute.update_rate() };
				attr->Dirty = true;
			}
		}

		// Add props.
		for (int i = 0; i < packet.properties_size(); ++i)
		{
			const auto& prop = packet.properties(i);
			auto soprop = so->Properties.Get(prop.name());
			if (soprop)
			{
				switch (prop.value_case())
				{
					case packet::Attribute::kAsInt:
						soprop->Set(prop.as_int());
						break;
					case packet::Attribute::kAsDouble:
						soprop->Set(prop.as_double());
						break;
					case packet::Attribute::kAsString:
						soprop->Set(prop.as_string());
						break;
				}

				soprop->Replicated = prop.replicated();
				soprop->NetworkUpdateRate = std::chrono::milliseconds{ prop.update_rate() };
				soprop->Dirty = true;
			}
		}

		// Add tilesets.
		if (so->GetType() == SceneObjectType::TMX)
		{
			auto tmx = std::dynamic_pointer_cast<TMXSceneObject>(so);
			for (int i = 0; i < packet.tileset_gids_size(); ++i)
			{
				const auto& tileset_gid = packet.tileset_gids(i);
				const auto& name = tileset_gid.name();
				const auto first_gid = tileset_gid.first_gid();
				const auto last_gid = tileset_gid.last_gid();

				auto tileset = server.GetTileset(name);
				if (tileset)
				{
					scene::TilesetGID gids{ first_gid, last_gid };
					tmx->Tilesets.emplace_back(std::make_pair(std::move(gids), tileset));
				}
			}
		}

		// Add to the server.
		server.AddSceneObject(so);

		// Add it to the appropriate scene.
		scene->AddObject(so);
	}
}

void handle(Server& server, const uint16_t playerId, const packet::SceneObjectChange& packet)
{
	const auto pid = packet.id();
	const auto pattached_to = packet.attached_to();
	const auto preplicatechanges = packet.replicatechanges();
	const auto pignoresevents = packet.ignoresevents();

	auto so = server.GetSceneObjectById(pid);
	if (!so) return;

	// If we own this scene object, don't allow changes.
	if (so->IsOwnedBy(server.GetPlayer()))
		return;

	// Set attached to.
	// Do this before we read in attributes/props as attaching sets the position to {0, 0}.
	if (pattached_to != 0)
	{
		auto attached = server.GetSceneObjectById(pattached_to);
		if (attached)
			so->AttachTo(attached);
	}

	so->ReplicateChanges = preplicatechanges;
	so->IgnoresEvents = pignoresevents;

	for (int i = 0; i < packet.attributes_size(); ++i)
	{
		const auto& packet_attribute = packet.attributes(i);
		if (auto attribute = load_attribute_from_packet(packet_attribute, so->Attributes); attribute)
		{
			// Force dirty flag.
			attribute->Dirty = true;
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
			case packet::Attribute::kAsInt:
				soprop->Set(prop.as_int());
				break;
			case packet::Attribute::kAsDouble:
				soprop->Set(prop.as_double());
				break;
			case packet::Attribute::kAsString:
				soprop->Set(prop.as_string());
				break;
		}

		soprop->Replicated = prop.replicated();
		soprop->NetworkUpdateRate = std::chrono::milliseconds{ prop.update_rate() };

		// Force dirty flag.
		// If something came across in this packet, it should force an update.  It might be an animation change.
		// Some animation systems (gani) will restart the animation if the animation prop was set to itself.
		soprop->Dirty = true;
	}

	// Physics updates.
	so->SynchronizePhysics();

	// Send changes to other players.
	auto scene = so->GetCurrentScene().lock();
	if (scene)
	{
		auto origin = server.GetPlayerById(playerId);
		server.SendToScene(scene, so->GetPosition(), PACKETID(Packets::SCENEOBJECTCHANGE), network::Channel::RELIABLE, packet, { server.GetPlayer(), origin });
	}
}

void handle(Server& server, const uint16_t playerId, const packet::SceneObjectDelete& packet)
{
	const auto pid = packet.id();

	if (!server.IsShuttingDown())
		log::PrintLine("<- Deleting scene object {}", pid);

	server.DeleteSceneObject(pid);
}

void handle(Server& server, const uint16_t playerId, const packet::SceneObjectOwnership& packet)
{
	const auto sceneobject_id = packet.sceneobject_id();
	const auto old_player_id = packet.old_player_id();
	const auto new_player_id = packet.new_player_id();

	// log::PrintLine("<- SceneObjectOwnership: Player {} takes ownership of {} from player {}.", new_player_id, sceneobject_id, old_player_id);

	auto so = server.GetSceneObjectById(sceneobject_id);
	auto old_player = server.GetPlayerById(old_player_id);
	auto new_player = server.GetPlayerById(new_player_id);

	if (!so)
		return;

	if (!server.IsSinglePlayer())
	{
		so->SetOwningPlayer(new_player);

		if (new_player) new_player->FollowedSceneObjects.insert(sceneobject_id);

		so->OnOwnershipChange.RunAll(new_player, old_player);
	}
}

void handle(Server& server, const uint16_t playerId, const packet::SceneObjectControl& packet)
{
	const auto& player_id = packet.player_id();
	const auto& sceneobject_id = packet.sceneobject_id();

	auto player = server.GetPlayerById(player_id);
	auto so = server.GetSceneObjectById(sceneobject_id);
	if (!player || !so) return;

	// If the player doesn't own the scene object, don't accept this change.
	if (!so->IsOwnedBy(player))
		return;

	// log::PrintLine("<-- SceneObjectControl: Player {} takes control of {}.", player_id, sceneobject_id);

	player->SwitchControlledSceneObject(so);
}

void handle(Server& server, const uint16_t playerId, const packet::SceneObjectUnfollow& packet)
{
	auto player = server.GetPlayerById(playerId);

	for (int i = 0; i < packet.id_size(); ++i)
	{
		const auto& oid = packet.id(i);
		player->FollowedSceneObjects.erase(oid);

		log::PrintLine("<- Removing scene object {} from player {}.", oid, playerId);
	}
}

void handle(Server& server, const uint16_t playerId, const packet::SceneObjectShapes& packet)
{
	if (!packet.has_body()) return;

	const auto sceneobject = packet.id();
	auto player = server.GetPlayerById(playerId);
	auto so = server.GetSceneObjectById(sceneobject);
	if (!player || !so) return;

	// TODO: Needs player id rework.
	// If the player who sent this packet doesn't own the scene object, don't accept this change.
	//if (!so->IsOwnedBy(player))
		//return;

	log::PrintLine("<- Received collision data for scene object {}.", sceneobject);
	readCollisionShapesFromPacket(so, packet, so->IsOwnedBy(server.GetPlayer()));
}

void handle(Server& server, const uint16_t playerId, const packet::SceneObjectChunkData& packet)
{
	const auto sceneobject = packet.id();
	auto player = server.GetPlayerById(playerId);
	auto so = server.GetSceneObjectById(sceneobject);
	if (!player || !so) return;

	// Currently, only TMX scene objects are supported.
	if (so->GetType() != SceneObjectType::TMX)
		return;

	auto tmx = std::dynamic_pointer_cast<TMXSceneObject>(so);

	size_t chunk_idx = packet.index();
	size_t max_chunks = packet.max_count();
	Vector2di chunk_position{ packet.pos_x(), packet.pos_y() };
	Vector2du chunk_dimensions{ packet.width(), packet.height() };

	log::PrintLine("<- Received chunk {} data for scene object {}.", chunk_idx, sceneobject);
	tmx->LoadChunkData(chunk_idx, max_chunks, std::move(chunk_position), std::move(chunk_dimensions));
	tmx->RenderOrder = static_cast<tmx::RenderOrder>(packet.render_order());
}

void handle(Server& server, const uint16_t playerId, const packet::SceneObjectRequestChunkData& packet)
{
	const auto sceneobject = packet.id();
	auto player = server.GetPlayerById(playerId);
	auto so = server.GetSceneObjectById(sceneobject);
	if (!player || !so) return;

	// Currently, only TMX scene objects are supported.
	if (so->GetType() != SceneObjectType::TMX)
		return;

	auto chunk_idx = packet.index();
	auto tmx = std::dynamic_pointer_cast<TMXSceneObject>(so);
	if (chunk_idx >= tmx->Chunks.size())
		return;

	log::PrintLine("<- Received request for chunk {} data for scene object {}.", chunk_idx, sceneobject);

	// Record the chunk details.
	const auto& chunk = tmx->Chunks.at(chunk_idx);
	packet::SceneObjectChunkData chunk_packet{};
	chunk_packet.set_id(sceneobject);
	chunk_packet.set_index(chunk_idx);
	chunk_packet.set_max_count(static_cast<uint32_t>(tmx->Chunks.size()));
	chunk_packet.set_pos_x(chunk.Position.x);
	chunk_packet.set_pos_y(chunk.Position.y);
	chunk_packet.set_width(chunk.Size.x);
	chunk_packet.set_height(chunk.Size.y);
	chunk_packet.set_render_order(static_cast<uint32_t>(tmx->RenderOrder));

	// If we have the TMX map, add the tiles.
	if (auto tmx_map = tmx->GetMap(); tmx_map != nullptr)
	{
		const auto& layers = tmx_map->getLayers();
		if (tmx->Layer >= layers.size())
			return;

		const auto& layer = layers.at(tmx->Layer);
		if (layer->getType() != tmx::Layer::Type::Tile)
			return;

		const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();
		const auto& chunks = tilelayer.getChunks();
		if (chunk_idx >= chunks.size())
			return;

		const auto& chunk = chunks.at(chunk_idx);
		auto* tiles = chunk_packet.mutable_tiles();
		auto tile_ids = chunk.tiles | std::ranges::views::transform([](const tmx::TileLayer::Tile& tile) -> const uint32_t { return tile.ID; });

		// Protobuf calculating iterator distances with ints again.
		#pragma warning (push)
		#pragma warning (disable : 4244)

		tiles->Assign(tile_ids.begin(), tile_ids.end());

		#pragma warning (pop)
	}

	// Send the data back to the requester.
	server.GetNetwork().Send(playerId, network::PACKETID(Packets::SCENEOBJECTCHUNKDATA), network::Channel::RELIABLE, chunk_packet);

	// Construct the shapes for the chunk too.
	if (so->GetPhysicsBodyConfiguration().has_value())
	{
		auto packet_collision = network::constructCollisionShapesPacket(so, so->GetPhysicsBodyConfiguration().value(), chunk_idx);
		server.GetNetwork().Send(playerId, network::PACKETID(Packets::SCENEOBJECTSHAPES), network::Channel::RELIABLE, packet_collision);
	}
}

void handle(Server& server, const uint16_t playerId, const packet::SceneTilesetAdd& packet)
{
	if (packet.tilesets_size() == 0)
		return;

	for (int i = 0; i < packet.tilesets_size(); ++i)
	{
		const auto& tileset = packet.tilesets(i);
		const auto& name = tileset.name();

		if (server.GetTileset(name) != nullptr)
			continue;

		scene::Tileset ts
		{
			.File = name,
			.TileDimensions = { tileset.tilewidth(), tileset.tileheight() },
			.TileCount = tileset.tilecount(),
			.Columns = static_cast<uint16_t>(tileset.columns()),
			.Rows = static_cast<uint16_t>(tileset.rows()),
			.Margin = static_cast<uint8_t>(tileset.margin()),
			.Spacing = static_cast<uint8_t>(tileset.spacing()),
		};

		server.AddTileset(name, std::make_shared<scene::Tileset>(std::move(ts)));
	}
}

void handle(Server& server, const uint16_t playerId, const packet::SendEvent& packet)
{
	if (server.IsSinglePlayer()) return;
	if (server.IsHost() && playerId == server.GetPlayer()->GetPlayerId())
		return;

	const auto sender = packet.sender();
	const auto& pscene = packet.scene();
	const auto& name = packet.name();
	const auto& data = packet.data();

	auto player = server.GetPlayerById(sender);
	auto scene = server.GetScene(pscene);

	if (packet.has_x() && packet.has_y() && packet.has_radius())
	{
		if (!scene)
			return;

		const auto x = packet.x();
		const auto y = packet.y();
		const auto radius = packet.radius();
		Vector2df origin{ x, y };

		log::PrintLine("<- Event '{}' from player {} to scene '{}' at ({}, {}):{} with data: {}", name, sender, pscene, x, y, radius, data);

		// Perform the event.
		auto hits = server.ProcessSendEvent(scene, player, name, data, origin, radius);
		log::PrintLine("   -- hits: {}", hits);
	}
	else
	{
		if (scene)
			log::PrintLine("<- Level event '{}' from player {} to scene '{}' with data: {}", name, sender, pscene, data);
		else log::PrintLine("<- Server event '{}' from player {} with data: {}", name, sender, data);

		// Perform the event.
		auto hits = server.ProcessSendEvent(scene, player, name, data);
		log::PrintLine("   -- hits: {}", hits);
	}
}

void handle(Server& server, const uint16_t playerId, const packet::FlagSet& packet)
{
	auto player = server.GetPlayerById(playerId);
	if (player == nullptr) return;
	if (player == server.GetPlayer()) return;

	for (int i = 0; i < packet.attributes_size(); ++i)
	{
		const auto& packet_attribute = packet.attributes(i);
		if (auto flag = load_attribute_from_packet(packet_attribute, player->Account.Flags); flag)
		{
			flag->Dirty = true;
			flag->ProcessDirty();
		}
	}
}

void handle(Server& server, const uint16_t playerId, const packet::ItemDefinition& packet)
{
	if (server.IsHost() || server.IsSinglePlayer())
		return;

	ItemID baseId = static_cast<ItemID>(packet.baseid());
	std::string name = packet.name();
	std::string description = packet.description();
	std::string image = packet.image();
	std::string clientScript = packet.clientscript();

	std::vector<std::string> tags;
	for (int i = 0; i < packet.tags_size(); ++i)
		tags.emplace_back(packet.tags(i));

	auto item = std::make_unique<item::ItemDefinition>(std::move(baseId), std::move(name), std::move(description), std::move(image), std::move(clientScript), std::move(tags));
	server.AddItemDefinition(std::move(item));
}

void handle(Server& server, const uint16_t playerId, const packet::ItemAdd& packet)
{
	auto player = server.GetPlayer();
	if (player == nullptr) return;

	ItemID id = static_cast<ItemID>(packet.id());
	ItemID baseId = static_cast<ItemID>(packet.baseid());
	item::ItemType type = static_cast<item::ItemType>(packet.type());
	size_t count = std::min(1ull, packet.stackable_count());

	auto item_definition = server.GetItemDefinition(baseId);
	if (item_definition == nullptr) return;

	// Host/SinglePlayer already has the item loaded so just run the scripts.
	// We would only get this packet on startup.
	if (server.IsHost() || server.IsSinglePlayer())
	{
		if (auto item = player->GetItem(id); item)
		{
			server.Script->RunScript(std::format("item_{}", id), item_definition->ClientScript, item);
			item->OnCreated.RunAll();
		}
		return;
	}

	auto hasExisting = player->HasItem(id);

	// Single items only have one instance.
	// Don't accept another.
	if (hasExisting && type == item::ItemType::SINGLE)
		return;

	// Get or create our item.
	item::ItemInstancePtr item = nullptr;
	if (hasExisting) item = player->GetItem(id);
	else
	{
		switch (type)
		{
			case item::ItemType::SINGLE: item = std::make_shared<item::ItemInstance>(id, baseId); break;
			case item::ItemType::STACKABLE: item = std::make_shared<item::ItemStackable>(id, baseId); break;
			case item::ItemType::VARIANT: item = std::make_shared<item::ItemVariant>(id, baseId); break;
		}
		item->ItemBase = item_definition;
		player->Account.AddItem(item);
	}

	// Load stackable.
	if (auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(item); stackable)
		stackable->Count = count;

	// Load variant.
	if (auto variant = std::dynamic_pointer_cast<item::ItemVariant>(item); variant)
	{
		for (int i = 0; i < packet.variant_attributes_size(); ++i)
		{
			const auto& variant_attribute = packet.variant_attributes(i);
			if (auto attribute = load_attribute_from_packet(variant_attribute, variant->VariantAttributes); attribute)
			{
				attribute->Dirty = false;
			}
		}
	}

	// Run OnCreated.
	if (!hasExisting)
	{
		server.Script->RunScript(std::format("item_{}", id), item_definition->ClientScript, item);
		item->OnCreated.RunAll();
	}

	// Run OnAdded.
	item->OnAdded.RunAll();
}

void handle(Server& server, const uint16_t playerId, const packet::ItemDelete& packet)
{
	if (server.IsHost() || server.IsSinglePlayer())
		return;

	ItemID id = static_cast<ItemID>(packet.id());
	size_t count = static_cast<size_t>(packet.count());

	if (auto item = server.GetPlayer()->GetItem(id); item)
	{
		server.RemoveItemFromPlayer(server.GetPlayer(), id, count);
		item->OnRemoved.RunAll(count);
	}
}

} // end namespace tdrp::handlers
