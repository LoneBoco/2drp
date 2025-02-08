#include <iostream>
#include <unordered_set>
#include <filesystem>
#include <format>

#include "engine/common.h"

#include "engine/os/Utils.h"

#include "engine/server/Server.h"
#include "engine/server/Account.h"
#include "engine/loader/Loader.h"
#include "engine/filesystem/File.h"
#include "engine/network/Packet.h"
#include "engine/network/PacketsInc.h"
#include "engine/network/PacketHandler.h"
#include "engine/network/construct/Attributes.h"
#include "engine/network/construct/SceneObject.h"
#include "engine/network/construct/Class.h"
#include "engine/network/construct/Tileset.h"

using tdrp::network::PACKETID;
using tdrp::network::Packets;

namespace tdrp::server
{
constexpr std::string_view SCRIPT_INSTANCE_NAME = "Server";

static std::vector<uint8_t> _serializeMessageToVector(const google::protobuf::Message& message)
{
	size_t message_size = message.ByteSizeLong();

	std::vector<uint8_t> result(message_size);
	message.SerializeToArray(result.data(), static_cast<int>(message_size));

	return result;
}

static bool _sendNewSceneObjectToPlayer(Server& server, const PlayerPtr& player, const SceneObjectPtr& so)
{
	if (so->IsLocal() || player->FollowedSceneObjects.contains(so->ID))
		return false;

	auto packet_newso = network::constructSceneObjectPacket(so);
	server.Send(player->GetPlayerId(), PACKETID(network::Packets::SCENEOBJECTNEW), network::Channel::RELIABLE, packet_newso);

	// Send collision.
	if (so->GetType() != SceneObjectType::TMX && so->GetPhysicsBodyConfiguration().has_value())
	{
		auto packet_collision = network::constructCollisionShapesPacket(so, so->GetPhysicsBodyConfiguration().value());
		server.Send(player->GetPlayerId(), PACKETID(network::Packets::SCENEOBJECTSHAPES), network::Channel::RELIABLE, packet_collision);
	}

	// Send chunk data.
	if (so->GetType() == SceneObjectType::TMX)
	{
		auto tmx = std::dynamic_pointer_cast<TMXSceneObject>(so);
		uint32_t chunk_count = static_cast<uint32_t>(tmx->Chunks.size());
		for (uint32_t chunk_idx = 0; chunk_idx < chunk_count; ++chunk_idx)
		{
			const auto& chunk = tmx->Chunks.at(chunk_idx);

			// Send the chunk details.
			packet::SceneObjectChunkData chunk_packet{};
			chunk_packet.set_id(so->ID);
			chunk_packet.set_index(chunk_idx);
			chunk_packet.set_max_count(chunk_count);
			chunk_packet.set_pos_x(chunk.Position.x);
			chunk_packet.set_pos_y(chunk.Position.y);
			chunk_packet.set_width(chunk.Size.x);
			chunk_packet.set_height(chunk.Size.y);
			server.Send(player->GetPlayerId(), PACKETID(network::Packets::SCENEOBJECTCHUNKDATA), network::Channel::RELIABLE, chunk_packet);
		}
	}

	return true;
}

static bool _sendDeleteSceneObjectToPlayers(Server& server, const scene::ScenePtr& scene, const SceneObjectPtr& so, const PlayerPtr& deleted_by)
{
	if (so->IsLocal() || !scene)
		return false;

	packet::SceneObjectDelete packet;
	packet.set_id(so->ID);
	server.BroadcastToScene(scene, PACKETID(network::Packets::SCENEOBJECTDELETE), network::Channel::RELIABLE, packet, { deleted_by });
	return true;
}

static bool _sendDeleteSceneObjectToPlayers(Server& server, const SceneObjectPtr& so, const PlayerPtr& deleted_by)
{
	return _sendDeleteSceneObjectToPlayers(server, so->GetCurrentScene().lock(), so, deleted_by);
}

/////////////////////////////

Server::Server()
	: m_connecting(false), m_server_type(ServerType::HOST), m_server_flags(0), m_server_name("PEER"), m_max_players(8)
{
	// Start player ids at 1.
	m_player_ids.SetNextUnusedId(1);

	auto script_manager = BabyDI::Get<script::ScriptManager>();
	Script = script_manager->CreateScriptInstance(std::string{ SCRIPT_INSTANCE_NAME });
	Script->BindIntoMe(
		[this](sol::state& lua) { lua["Server"] = this; }
	);

	// Make up unique id.
	m_unique_id = tdrp::os::CalculateComputerName();

	// Create blank object class.
	m_object_classes.insert(std::make_pair("blank", std::make_shared<ObjectClass>("blank")));

	// Bind the network callbacks.
	using namespace std::placeholders;
	m_network.AddConnectCallback(std::bind(&Server::network_connect, this, _1));
	m_network.AddDisconnectCallback(std::bind(&Server::network_disconnect, this, _1));
	m_network.AddLoginCallback(std::bind(&Server::network_login, this, _1, _2, _3, _4));
	m_network.AddReceiveCallback(std::bind(network::handlers::network_receive_server, std::ref(*this), _1, _2, _3, _4));
	m_network.BindServer(this);

	// Bind the accounts directory.
	FileSystem.Bind(fs::FileCategory::ACCOUNTS, filesystem::path("accounts") / GetUniqueId());
}

Server::~Server()
{
	Shutdown();
}

bool Server::Initialize(const ServerType type, const uint16_t flags)
{
	network::Network::Shutdown();
	if (!network::Network::Startup())
		return false;

	m_server_type = type;
	m_server_flags = flags;

	log::PrintLine(log::game, ":: Initialized server.");
	return true;
}

bool Server::LoadPackage(const std::string& package_name)
{
	if (m_package != nullptr)
		throw std::runtime_error("!! Tried to load a package when one is already loaded!  Destroy the old Server and make a new one.");

	log::PrintLine(log::game, ":: Loading package: {}", package_name);
	auto indent = log::game.indent();

	// Load the package.
	auto [load_success, package] = Loader::LoadPackageIntoServer(*this, package_name);
	m_package = package;

	// Sanity check for starting scene.
	if (package->GetStartingScene().empty())
		throw std::runtime_error("!! No starting scene for package!");

	// Load everything from the package into the server.
	if (HASFLAG(m_server_flags, ServerFlags::PRELOAD_EVERYTHING))
	{
		log::PrintLine(log::game, "Loading all scenes in package.");
		auto indent = log::game.indent();

		// Load all scenes.
		for (const std::filesystem::path& directory : FileSystem.GetDirectories(fs::FileCategory::LEVELS))
		{
			for (auto& dir : filesystem::directory_iterator{ directory })
			{
				if (!filesystem::is_directory(dir.status()))
					continue;

				auto scene = Loader::CreateScene(*this, m_scenes, dir.path().filename().string(), dir);
			}
		}
	}
	// Load only the starting scene into the server.
	else
	{
		log::PrintLine(log::game, "Loading the starting scene: {}.", package->GetStartingScene());

		// Load our starting scene.
		if (auto path = FileSystem.GetDirectoryPath(fs::FileCategory::LEVELS, package->GetStartingScene()); !path.empty())
			auto scene = Loader::CreateScene(*this, m_scenes, package->GetStartingScene(), path);
		else return false;
	}

	// Load server script.
	if (IsHost())
	{
		log::PrintLine(log::game, "Loading server scripts:");
		auto indent = log::game.indent();
		for (const auto& [name, script] : m_server_scripts)
		{
			log::PrintLine(log::game, "{}", name);
			Script->RunScript(name, script, this);
		}
	}

	// Run our started script.
	OnStarted.RunAll();
	return true;
}

bool Server::HostDedicated(const uint16_t port, const size_t peers)
{
	m_server_type = ServerType::DEDICATED;
	UNSETFLAG(m_server_flags, ServerFlags::SINGLEPLAYER);

	log::PrintLine(log::game, ":: Hosting on port {} for {} peers.", port, peers);

	m_network.Initialize(peers, port);
	return true;
}

bool Server::Host(const uint16_t port, const size_t peers)
{
	m_server_type = ServerType::HOST;
	UNSETFLAG(m_server_flags, ServerFlags::SINGLEPLAYER);

	log::PrintLine(log::game, ":: Hosting on port {} for {} peers.", port, peers);

	// Initialize our network and connect.
	m_network.Initialize(peers, port);

	// Log the player in.
	auto id = GetNextPlayerID();
	SetPlayerNumber(id);
	ProcessPlayerLogin(id, "host");
	ProcessPlayerJoin(id);

	return true;
}

bool Server::Connect(const std::string& hostname, const uint16_t port)
{
	m_server_type = ServerType::GUEST;
	m_server_flags = 0;

	log::PrintLine(log::game, ":: Connecting to {} on port {}.", hostname, port);

	m_network.Initialize();
	m_connecting = true;
	m_connecting_future = m_network.Connect(hostname, port);
	return true;
}

bool Server::SinglePlayer()
{
	m_server_type = ServerType::HOST;
	SETFLAG(m_server_flags, ServerFlags::SINGLEPLAYER);

	log::PrintLine(log::game, ":: Starting as single player server.");

	// Log the player in.
	auto id = GetNextPlayerID();
	SetPlayerNumber(id);
	ProcessPlayerLogin(id, "singleplayer");
	ProcessPlayerJoin(id);

	return true;
}

/////////////////////////////

void Server::Shutdown()
{
	m_shutting_down = true;

	if (!network::Network::IsStarted())
		return;

	// Disconnect.
	// Script object will be erased in network_disconnect.
	while (!m_player_list.empty())
	{
		auto player = m_player_list.begin();
		network_disconnect(player->first);
	}
	m_player = nullptr;

	// Disconnect.
	m_network.Disconnect();

	// Shut down the network.
	network::Network::Shutdown();

	// Erase script instance on every scene object.
	for (auto& [k, scene] : m_scenes)
	{
		for (auto& [k, so] : scene->GetGraph())
		{
			SCRIPT_THEM_ERASE(so);
		}
	}

	// Clear the scenes.
	m_scenes.clear();

	// Delete us from the script instance.
	auto script_manager = BabyDI::Get<script::ScriptManager>();
	script_manager->EraseScriptInstance(std::string{ SCRIPT_INSTANCE_NAME });

	// Erase script instance.
	SCRIPT_ME_ERASE;
}

/////////////////////////////

void Server::PreUpdate()
{
	// Check for connection status.
	if (m_connecting)
	{
		if (m_connecting_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			m_connecting = false;

			bool success = m_connecting_future.get();
			if (success)
			{
				log::PrintLine(log::game, ":: Connection was successful, sending login packet.");

				packet::Login packet;
				packet.set_method(packet::Login_Method_PEER2PEER);
				packet.set_account(m_credential_account);
				packet.set_passwordhash(m_credential_password_hash);
				packet.set_nickname(m_credential_nickname);
				packet.set_type(0);
				packet.set_version("first");

				Send(PACKETID(Packets::LOGIN), network::Channel::RELIABLE, packet);
			}
			else
			{
				log::PrintLine(log::game, "!! Connection failed.");
				m_network.Disconnect();
			}
		}
	}
	else
	{
		// Collect incoming data.
		if (!IsSinglePlayer())
			m_network.Update();
	}
}

void Server::Update(const std::chrono::milliseconds& tick)
{
	auto tick_count = tick.count();

	// Run server update script.
	OnServerTick.RunAll(tick_count);

	// Iterate through all the players and determine if any new scene objects should be sent.
	// While iterating through players, also deal with flag changes.
	// TODO: Check if a guest should be doing this.
	for (auto& [id, player] : m_player_list)
	{
		if (player == nullptr)
			continue;

		// Check our flags.
		if (player->Account.Flags.HasDirty())
		{
			packet::FlagSet message;
			network::processAndAssignAttributesToPacket(message, player->Account.Flags, &packet::FlagSet::add_attributes, tick);
			if (IsHost() && !IsSinglePlayer() && player != m_player)
				Send(player->GetPlayerId(), PACKETID(network::Packets::FLAGSET), network::Channel::RELIABLE, message);
		}

		// Don't send to ourselves.
		if (player == m_player)
			continue;

		auto pso = player->GetCurrentControlledSceneObject().lock();
		auto scene = player->GetCurrentScene().lock();
		if (pso && scene)
		{
			// Get all scene objects nearby.
			auto nearby = scene->FindObjectsInRangeOf(pso->GetPosition(), scene->GetTransmissionDistance() * 1.5f);

			std::set<SceneObjectID> new_objects;

			// Find every one not known about.
			//for (auto& so : nearby | std::views)
			auto& p = player;
			std::for_each(std::begin(nearby), std::end(nearby), [&](const decltype(nearby)::value_type& item)
			{
				if (!_sendNewSceneObjectToPlayer(*this, player, item))
					return;

				log::PrintLine(log::game, "-> Sending scene object {} via range to player {}.", item->ID, player->GetPlayerId());
				new_objects.insert(item->ID);
			});

			player->FollowedSceneObjects.insert(std::begin(new_objects), std::end(new_objects));
		}
	}

	// Iterate through all the scenes and see if we should unfollow any scene objects.

	// Iterate through all the scenes and update all the scene objects in them.
	for (auto& [name, scene] : m_scenes)
	{
		// Update the physics in the scene.
		scene->Physics.Update(tick);

		for (auto& [id, object] : scene->m_graph)
		{
			// Run the server tick script on the object.
			object->OnUpdate.RunAll(tick_count);

			// Process our attributes.
			packet::SceneObjectChange packet;
			network::processAndAssignAttributesToPacket(packet, object->Attributes, &packet::SceneObjectChange::add_attributes, tick);
			network::processAndAssignAttributesToPacket(packet, object->Properties, &packet::SceneObjectChange::add_properties, tick);

			// If nothing is being sent, we can continue to the next object.
			if (packet.attributes_size() == 0 && packet.properties_size() == 0)
				continue;

			// If we are the host and this object doesn't have an owner, we'll take care of the events.
			if (IsHost() && object->GetOwningPlayer() == NO_PLAYER)
				SwitchSceneObjectOwnership(object, m_player);

			// If the scene object is not replicated across the network, don't send anything.
			if (!object->ReplicateChanges)
				continue;

			// If we are in single player mode, there is no need to send anything.
			if (IsSinglePlayer())
				continue;

			// If we don't own this scene object, don't send anything.
			if (object->GetOwningPlayer() != m_player->GetPlayerId())
				continue;

			// Send the packet to all players in the scene.
			packet.set_id(object->ID);
			packet.set_replicatechanges(object->ReplicateChanges);
			packet.set_ignoresevents(object->IgnoresEvents);
			packet.set_attached_to(object->GetAttachedId());
			auto location = object->GetPosition();
			SendToScene(scene, location, PACKETID(Packets::SCENEOBJECTCHANGE), network::Channel::RELIABLE, packet, { m_player });

			// Send over any physics changes to the scene object.
			// This is changes in collision mesh or simulated body type, not velocities or anything.
			if (object->PhysicsChanged)
			{
				object->PhysicsChanged = false;

				// Don't send TMX physics over the network this way.
				if (object->GetType() != SceneObjectType::TMX && object->GetPhysicsBodyConfiguration().has_value())
				{
					auto packet = network::constructCollisionShapesPacket(object, object->GetPhysicsBodyConfiguration().value());
					SendToScene(scene, object->GetPosition(), PACKETID(Packets::SCENEOBJECTSHAPES), network::Channel::RELIABLE, packet, { m_player });
				}
			}
		}
	}

	if (!IsSinglePlayer() && !m_connecting)
		m_network.Update();

	FileSystem.Update();
}

void Server::ProcessPlayerLogin(const uint16_t player_id, const std::string& account)
{
	// Load account.
	log::PrintLine(log::game, ":: Loading account {} for player {}.", account, player_id);
	auto player = GetPlayerById(player_id);
	player->Account.Load(account);

	// Tell the client the login was a success.
	packet::LoginStatus login_status;
	login_status.set_player_id(player_id);
	login_status.set_success(true);
	login_status.set_host_player_id(m_player->GetPlayerId());
	Send(player_id, PACKETID(Packets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
	log::PrintLine(log::game, "-> [LOGINSTATUS] Sending login allowed.");

	// Send classes.
	for (const auto& [name, oc] : m_object_classes)
	{
		auto packet = network::constructClassAddPacket(oc);
		Send(player_id, PACKETID(Packets::CLASSADD), network::Channel::RELIABLE, packet);
	}

	// Send tilesets.
	for (const auto& [_, tileset] : m_tilesets)
		Send(player_id, PACKETID(Packets::SCENETILESETADD), network::Channel::RELIABLE, network::constructTilesetAddPacket(tileset));

	// Send client scripts.
	for (const auto& [name, script_pair] : m_client_scripts)
	{
		// Send when required (first pair) or when the player has the script.
		bool send = script_pair.first || player->Account.ClientScripts.contains(script_pair.second);
		if (send)
			AddPlayerClientScript(name, player);
	}

	// Send owned items.
	for (const auto& [itemId, item] : player->Account.Items)
	{
		// Send item definitions.
		SendItemDefinition(player, item->ItemBaseID);

		// Send the item.
		packet::ItemAdd packetAdd;
		packetAdd.set_id(item->ID);
		packetAdd.set_baseid(item->ItemBaseID);
		packetAdd.set_type(static_cast<packet::ItemType>(item->Type));

		if (auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(item); stackable)
			packetAdd.set_stackable_count(stackable->Count);

		if (auto variant = std::dynamic_pointer_cast<item::ItemVariant>(item); variant)
			network::assignAllAttributesToPacket(packetAdd, variant->VariantAttributes, &packet::ItemAdd::add_variant_attributes);

		log::PrintLine(log::game, "-> [ITEMADD] Sending weapon {} (base {}, type {}) to player {}.", item->ID, item->ItemBaseID, static_cast<uint8_t>(item->Type), player->GetPlayerId());
		Send(player->GetPlayerId(), network::PACKETID(network::Packets::ITEMADD), network::Channel::RELIABLE, packetAdd);
	}

	// Send server info.
	// Keep this last.  The client will start the load after this packet.
	packet::ServerInfo server_info;
	server_info.set_uniqueid(m_unique_id);
	server_info.set_package(m_package->GetName());
	server_info.set_loadingscene(m_package->GetLoadingScene());
	/*
	if (m_package != nullptr)
	{
		auto file_details = FileSystem.GetArchiveInfo(m_package->GetBasePath());
		for (auto& detail : file_details)
		{
			auto* file = server_info.add_files();
			file->set_name(detail.File.filename().string());
			file->set_size(detail.FileSize);
			file->set_crc32(detail.CRC32);
			file->set_date(detail.TimeSinceEpoch);
		}
	}
	*/
	Send(player_id, PACKETID(Packets::SERVERINFO), network::Channel::RELIABLE, server_info);
}

void Server::ProcessPlayerJoin(const uint16_t player_id)
{
	auto player = GetPlayerById(player_id);

	// Switch to the starting scene.
	auto scene = GetScene(player->Account.LastKnownScene);
	if (scene == nullptr)
		scene = GetOrCreateScene(GetPackage()->GetStartingScene());

	log::PrintLine(log::game, "-> Sending player {} to scene {}.", player_id, scene->GetName());
	player->SwitchScene(scene);

	// Call the OnPlayerJoin script function.
	OnPlayerJoin.RunAll(player);
}

/////////////////////////////

void Server::SetPlayerNumber(const uint16_t player_number)
{
	log::PrintLine(log::game, ":: Assigned player number {}.", player_number);

	//if (IsGuest() || player_number == 0)
	{
		auto player = std::make_shared<Player>(player_number);
		player->BindServer(this);
		m_player_list[player_number] = player;
		m_player = player;
	}
}

void Server::SetLoginCredentials(std::string_view account, std::string_view password_hash, std::string_view nickname)
{
	m_credential_account = account;
	m_credential_password_hash = password_hash;
	m_credential_nickname = nickname;
}

void Server::SetLoginCredentials(std::string_view nickname)
{
	m_credential_account = "guest";
	m_credential_password_hash = "guest";
	m_credential_nickname = nickname;
}

///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ObjectClass> Server::GetObjectClass(const std::string& name)
{
	auto iter = m_object_classes.find(name.empty() ? "blank" : name);
	if (iter == m_object_classes.end())
	{
		auto c = std::make_shared<ObjectClass>(name);
		m_object_classes.insert(std::make_pair(name, c));
		return c;
	}
	return iter->second;
}

std::shared_ptr<scene::Tileset> Server::GetTileset(const std::string& name) const
{
	auto iter = m_tilesets.find(name);
	if (iter == m_tilesets.end())
		return nullptr;
	return iter->second;
}

scene::ScenePtr Server::GetScene(const std::string& name) const
{
	auto iter = m_scenes.find(name);
	if (iter == m_scenes.end())
		return nullptr;
	return iter->second;
}

scene::ScenePtr Server::GetOrCreateScene(const std::string& name) noexcept
{
	auto scene = GetScene(name);
	if (scene != nullptr)
		return scene;

	scene = std::make_shared<scene::Scene>(name);
	m_scenes.insert(std::make_pair(name, scene));
	return scene;
}

///////////////////////////////////////////////////////////////////////////////

bool Server::SwitchPlayerScene(PlayerPtr& player, scene::ScenePtr& new_scene)
{
	if (player == nullptr || new_scene == nullptr)
		return false;

	auto old_scene = player->SwitchScene(new_scene);

	// Tell the player which scene they are on.
	packet::SwitchScene switchscene;
	switchscene.set_scene(new_scene->GetName());
	Send(player->GetPlayerId(), network::PACKETID(network::Packets::SWITCHSCENE), network::Channel::RELIABLE, switchscene);

	// Send scene objects to the player.
	// TODO: Use the actual starting position of the player.
	auto sceneobjects = new_scene->FindObjectsInRangeOf({ 0.0f, 0.0f }, static_cast<float>(new_scene->GetTransmissionDistance()));
	for (const auto& sceneobject : sceneobjects)
	{
		if (!_sendNewSceneObjectToPlayer(*this, player, sceneobject))
			continue;

		log::PrintLine(log::game, "-> Sending scene object {} via scene switch.", sceneobject->ID);
		player->FollowedSceneObjects.insert(sceneobject->ID);

		// If there is no owning player, take ownership.
		if (sceneobject->GetOwningPlayer() == NO_PLAYER)
			SwitchSceneObjectOwnership(sceneobject, player);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

void Server::LoadServerScript(const std::string& name, const std::string& script)
{
	auto it = m_server_scripts.find(name);
	if (it == std::end(m_server_scripts))
	{
		if (log::game.not_indented())
			log::Print(log::game, ":: Adding server script: ");

		log::PrintLine(log::game, name);
		m_server_scripts[name] = script;
	}
	else
	{
		if (log::game.not_indented())
			log::Print(log::game, ":: Replacing server script: ");

		log::PrintLine(log::game, name);
		m_server_scripts[name] = script;
	}

	// TODO: Proper script replacement.
	Script->RunScript(name, script, this);
}

void Server::LoadClientScript(const std::string& name, const std::string& script, bool required)
{
	auto it = m_client_scripts.find(name);
	if (it == std::end(m_client_scripts))
	{
		if (log::game.not_indented())
			log::Print(log::game, ":: Adding client script: ");

		log::PrintLine(log::game, name);
		m_client_scripts[name] = std::make_pair(required, script);
	}
	else
	{
		if (log::game.not_indented())
			log::Print(log::game, ":: Replacing client script: ");

		log::PrintLine(log::game, name);
		m_client_scripts[name] = std::make_pair(required, script);
	}

	// Send the changes to all players who have this script.
	if (IsHost())
	{
		for (auto& player : m_player_list)
		{
			if (required || player.second->Account.ClientScripts.contains(name))
			{
				packet::ClientScriptAdd packet;
				packet.set_name(name);
				packet.set_script(script);

				Send(player.first, PACKETID(Packets::CLIENTSCRIPTADD), network::Channel::RELIABLE, packet);
			}
		}
	}
}

void Server::EraseClientScript(const std::string& name)
{
	auto it = m_client_scripts.find(name);
	if (it == std::end(m_client_scripts))
		return;

	log::PrintLine(log::game, ":: Erasing client script \"{}\".", name);
	m_client_scripts.erase(it);

	// Send the changes to all players who have this script.
	if (IsHost())
	{
		for (auto& player : m_player_list)
		{
			if (player.second->Account.ClientScripts.contains(name))
			{
				packet::ClientScriptDelete packet;
				packet.set_name(name);

				Send(player.first, PACKETID(Packets::CLIENTSCRIPTDELETE), network::Channel::RELIABLE, packet);
			}
		}
	}
}

void Server::AddPlayerClientScript(const std::string& name, PlayerPtr player)
{
	if (player == nullptr) return;

	auto it = m_client_scripts.find(name);
	if (it == std::end(m_client_scripts))
		return;

	// If not a required script, register it to the account.
	if (!it->second.first)
		player->Account.AddClientScript(name);

	if (IsHost())
	{
		const auto& script = it->second.second;

		packet::ClientScriptAdd packet;
		packet.set_name(name);
		packet.set_script(script);

		log::PrintLine(log::game, "-> [CLIENTSCRIPTADD] Sending client script \"{}\" to player {}.", name, player->GetPlayerId());
		Send(player->GetPlayerId(), PACKETID(Packets::CLIENTSCRIPTADD), network::Channel::RELIABLE, packet);
	}
}

void Server::RemovePlayerClientScript(const std::string& name, PlayerPtr player)
{
	if (player == nullptr) return;

	auto it = m_client_scripts.find(name);
	if (it == std::end(m_client_scripts))
		return;

	player->Account.RemoveClientScript(name);

	if (IsHost())
	{
		packet::ClientScriptDelete packet;
		packet.set_name(name);

		log::PrintLine(log::game, "-> [CLIENTSCRIPTDELETE] Removing client script \"{}\" from player {}.", name, player->GetPlayerId());
		Send(player->GetPlayerId(), PACKETID(Packets::CLIENTSCRIPTDELETE), network::Channel::RELIABLE, packet);
	}
}

///////////////////////////////////////////////////////////////////////////////

bool Server::AddItemDefinition(item::ItemDefinitionUPtr&& item)
{
	if (!item) return false;
	auto baseID = item->BaseID;
	auto& name = item->Name;

	const auto&& [iter, success] = m_item_definitions.emplace(std::make_pair(item->BaseID, std::move(item)));
	if (success)
	{
		if (log::game.not_indented())
			log::Print(log::game, ":: Loaded item: ");

		log::PrintLine(log::game, "{}.{}", baseID, name);
		Script->RunScript(std::format("item_{}", baseID), iter->second->ClientScript, iter->second.get());
		iter->second->OnCreated.RunAll();
	}
	else log::PrintLine(log::game, "!! Failed to add item {}.{}. Item with the same ID already exists.", baseID, name);

	return success;
}

void Server::SendItemDefinition(server::PlayerPtr player, ItemID baseId)
{
	// Don't send the item definition if the player already knows it.
	if (player->KnowsItemDefinition(baseId))
		return;

	if (auto baseItem = GetItemDefinition(baseId); baseItem)
	{
		log::PrintLine(log::game, "-> Sending item definition {}.{} to player {}.", baseId, baseItem->Name, player->GetPlayerId());
		player->AddKnownItemDefinition(baseId);

		// Only send item definitions if we are the host.
		if (IsHost() && player != m_player)
		{
			packet::ItemDefinition packetDef;
			packetDef.set_baseid(baseItem->BaseID);
			packetDef.set_name(baseItem->Name);
			packetDef.set_image(baseItem->Image);
			packetDef.set_description(baseItem->Description);
			packetDef.set_clientscript(baseItem->ClientScript);
			for (const auto& tag : baseItem->Tags)
				packetDef.add_tags(tag);

			Send(player->GetPlayerId(), network::PACKETID(network::Packets::ITEMDEFINITION), network::Channel::RELIABLE, packetDef);
		}
	}
}

item::ItemDefinition* Server::GetItemDefinition(ItemID baseId)
{
	auto iter = m_item_definitions.find(baseId);
	if (iter == std::end(m_item_definitions))
		return nullptr;

	return iter->second.get();
}

item::ItemDefinition* Server::GetItemDefinition(const std::string& name)
{
	auto iter = std::ranges::find_if(m_item_definitions, [&name](decltype(m_item_definitions)::value_type& node) { return boost::iequals(node.second->Name, name); });
	if (iter == std::ranges::end(m_item_definitions))
		return nullptr;
	return iter->second.get();
}

item::ItemInstancePtr Server::GiveItemToPlayer(server::PlayerPtr player, ItemID baseId, item::ItemType type, size_t count)
{
	if (IsGuest() || player == nullptr || count == 0 || type == item::ItemType::VARIANT)
		return nullptr;

	auto* definition = GetItemDefinition(baseId);
	if (definition == nullptr)
		return nullptr;

	auto next_id = [&player]() -> ItemID
	{
		auto iter = player->Account.Items.rbegin();
		if (iter == std::rend(player->Account.Items))
			return 0;
		return iter->first + 1;
	};

	auto match_single = [&player, &baseId, &type](decltype(player->Account.Items)::value_type& node)
	{
		auto& item = node.second;
		if (item->ItemBaseID == baseId && item->Type == type)
			return true;
		return false;
	};

	auto host_script = [this, definition](item::ItemInstancePtr& item, const PlayerPtr& player, size_t count = 1, bool is_new = true)
	{
		if (m_player != player) return;
		definition->OnAdded.RunAll(item, count);
	};

	auto send_item = [this, &host_script](item::ItemInstancePtr& item, const PlayerPtr& player, size_t count = 1, bool is_new = true)
	{
		host_script(item, player, count, is_new);

		// If this is ourself, send a blank packet so our client can update the UI.
		if (m_player == player)
		{
			Send(player->GetPlayerId(), network::PACKETID(network::Packets::ITEMADD), network::Channel::RELIABLE);
			return;
		}

		// Try to send the item definition.
		SendItemDefinition(player, item->ItemBaseID);

		// Send the item.
		packet::ItemAdd packetAdd;
		packetAdd.set_id(item->ID);
		packetAdd.set_baseid(item->ItemBaseID);
		packetAdd.set_type(static_cast<packet::ItemType>(item->Type));

		if (auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(item); stackable)
			packetAdd.set_stackable_count(stackable->Count);

		if (auto variant = std::dynamic_pointer_cast<item::ItemVariant>(item); variant)
			network::assignAllAttributesToPacket(packetAdd, variant->VariantAttributes, &packet::ItemAdd::add_variant_attributes);

		Send(player->GetPlayerId(), network::PACKETID(network::Packets::ITEMADD), network::Channel::RELIABLE, packetAdd);
	};

	switch (type)
	{
		// Single items that only have one instance.
		case item::ItemType::SINGLE:
		{
			auto existing = std::ranges::find_if(player->Account.Items, match_single);
			if (existing != std::ranges::end(player->Account.Items))
				return existing->second;

			auto instance = std::make_shared<item::ItemInstance>(next_id(), baseId);
			instance->ItemBase = definition;

			player->Account.AddItem(instance);
			send_item(instance, player);
			return instance;
		}

		case item::ItemType::STACKABLE:
		{
			auto iter = std::ranges::find_if(player->Account.Items, match_single);
			if (iter != std::ranges::end(player->Account.Items))
			{
				auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(iter->second);
				auto& instance = iter->second;
				if (stackable) stackable->Count += count;
				instance->ItemBase = definition;

				send_item(instance, player, count, false);
				return instance;
			}
			else
			{
				auto stackable = std::make_shared<item::ItemStackable>(next_id(), baseId);
				auto instance = std::dynamic_pointer_cast<item::ItemInstance>(stackable);
				stackable->Count = count;
				stackable->ItemBase = definition;

				player->Account.AddItem(instance);
				send_item(instance, player, count);
				return instance;
			}
			break;
		}
	}

	return nullptr;
}

item::ItemInstancePtr Server::GiveVariantItemToPlayer(server::PlayerPtr player, ItemID baseId, const ObjectAttributes& attributes)
{
	if (IsGuest() || player == nullptr)
		return nullptr;

	auto* definition = GetItemDefinition(baseId);
	if (definition == nullptr)
		return nullptr;

	auto next_id = [&player]() -> ItemID
	{
		auto iter = player->Account.Items.rbegin();
		if (iter == std::rend(player->Account.Items))
			return 0;
		return iter->first + 1;
	};

	auto host_script = [this, definition](item::ItemInstancePtr& item, const PlayerPtr& player, size_t count = 1, bool is_new = true)
	{
		if (m_player != player) return;
		definition->OnAdded.RunAll(item, count);
	};

	auto send_item = [this, &host_script](item::ItemInstancePtr& item, const PlayerPtr& player, size_t count = 1, bool is_new = true)
	{
		host_script(item, player, count, is_new);

		// If this is ourself, send a blank packet so our client can update the UI.
		if (m_player == player)
		{
			Send(player->GetPlayerId(), network::PACKETID(network::Packets::ITEMADD), network::Channel::RELIABLE);
			return;
		}

		// Try to send the item definition.
		SendItemDefinition(player, item->ItemBaseID);

		// Send the item.
		packet::ItemAdd packetAdd;
		packetAdd.set_id(item->ID);
		packetAdd.set_baseid(item->ItemBaseID);
		packetAdd.set_type(static_cast<packet::ItemType>(item->Type));

		if (auto variant = std::dynamic_pointer_cast<item::ItemVariant>(item); variant)
			network::assignAllAttributesToPacket(packetAdd, variant->VariantAttributes, &packet::ItemAdd::add_variant_attributes);

		Send(player->GetPlayerId(), network::PACKETID(network::Packets::ITEMADD), network::Channel::RELIABLE, packetAdd);
	};

	if (!player->KnowsItemDefinition(baseId))
	{
		// Try to send the item definition.
		SendItemDefinition(player, baseId);
	}

	auto variant = std::make_shared<item::ItemVariant>(next_id(), baseId);
	variant->VariantAttributes = attributes;
	variant->ItemBase = definition;

	auto instance = std::dynamic_pointer_cast<item::ItemInstance>(variant);
	player->Account.AddItem(instance);
	send_item(instance, player);
	return instance;
}

item::ItemInstancePtr Server::RemoveItemFromPlayer(server::PlayerPtr player, ItemID itemId, size_t count)
{
	if (player == nullptr || IsGuest() && player != m_player)
		return nullptr;

	auto iter = player->Account.Items.find(itemId);
	if (iter == std::end(player->Account.Items))
		return nullptr;

	item::ItemInstancePtr item = iter->second;

	// If we are removing a stackable item, and the count is greater than the requested removal count, adjust the count since we won't be deleting it.
	if (item->Type == item::ItemType::STACKABLE)
	{
		if (auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(item); stackable != nullptr && stackable->Count > count)
			return SetItemCount(player, itemId, stackable->Count - count);
	}

	// Remove the item.
	player->Account.RemoveItem(item);

	// If this is for us, just run the script and return.
	if (player == m_player)
	{
		if (auto* base = item->ItemBase; base != nullptr)
			base->OnRemoved.RunAll(item, count);

		Send(player->GetPlayerId(), PACKETID(network::Packets::ITEMCOUNT), network::Channel::RELIABLE);
		return item;
	}

	// Send the delete to the player.
	if (IsHost())
	{
		packet::ItemCount packet;
		packet.set_id(item->ID);
		packet.set_count(0);

		Send(player->GetPlayerId(), PACKETID(network::Packets::ITEMCOUNT), network::Channel::RELIABLE, packet);
	}
	return item;
}

item::ItemInstancePtr Server::SetItemCount(server::PlayerPtr player, ItemID itemId, size_t count)
{
	if (player == nullptr || IsGuest() && player != m_player)
		return nullptr;

	auto iter = player->Account.Items.find(itemId);
	if (iter == std::end(player->Account.Items))
		return nullptr;

	item::ItemInstancePtr item = iter->second;

	// If this is not a stackable item, but our count is 0, delete it.
	if (item->Type != item::ItemType::STACKABLE && count == 0)
		return RemoveItemFromPlayer(player, itemId);

	// Adjust the stack size.
	if (auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(item); stackable)
	{
		size_t old_count = stackable->Count;
		stackable->Count = count;
		if (stackable->Count == 0)
			return RemoveItemFromPlayer(player, itemId);

		// Run functions to let us know that our item count changed.
		if (auto* base = stackable->ItemBase; base != nullptr)
		{
			if (old_count > count)
				base->OnRemoved.RunAll(item, old_count - count);
			else if (old_count < count)
				base->OnAdded.RunAll(item, count - old_count);
		}

		// Send the count adjustment to the player.
		if (IsHost())
		{
			packet::ItemCount packet;
			packet.set_id(item->ID);
			packet.set_count(count);

			Send(player->GetPlayerId(), PACKETID(network::Packets::ITEMCOUNT), network::Channel::RELIABLE, packet);
		}
	}

	return item;
}

///////////////////////////////////////////////////////////////////////////////

void Server::SetAccountFlag(server::PlayerPtr player, const std::string& flag, const auto& value)
{
	if (player == nullptr) return;

	auto f = player->Account.Flags.GetOrCreate(flag);
	f->Set(value);
}

AttributePtr Server::GetAccountFlag(const server::PlayerPtr player, const std::string& flag) const
{
	if (player == nullptr) return nullptr;

	auto f = player->Account.Flags.Get(flag);
	return f;
}

///////////////////////////////////////////////////////////////////////////////

SceneObjectPtr Server::CreateSceneObject(SceneObjectType type, const std::string& object_class, SceneObjectID id)
{
	// Get our object class.
	auto oc = GetObjectClass(object_class);
	if (oc == nullptr)
		return nullptr;

	// Retrieve the new ID for our scene object.
	auto soid = id != 0 ? id : GetNextSceneObjectID();

	// Create the scene object.
	SceneObjectPtr so = nullptr;
	switch (type)
	{
	case SceneObjectType::DEFAULT:
		so = std::make_shared<tdrp::SceneObject>(oc, soid);
		break;
	case SceneObjectType::STATIC:
		so = std::make_shared<tdrp::StaticSceneObject>(oc, soid);
		break;
	case SceneObjectType::ANIMATED:
		so = std::make_shared<tdrp::AnimatedSceneObject>(oc, soid);
		break;
	case SceneObjectType::TILEMAP:
		so = std::make_shared<tdrp::TiledSceneObject>(oc, soid);
		break;
	case SceneObjectType::TMX:
		so = std::make_shared<tdrp::TMXSceneObject>(oc, soid);
		break;
	case SceneObjectType::TEXT:
		so = std::make_shared<tdrp::TextSceneObject>(oc, soid);
		break;
	default:
		log::PrintLine(log::game, "!! CreateSceneObject requires a valid scene object type.");
		return nullptr;
	}

	return so;
}

SceneObjectPtr Server::AddSceneObject(SceneObjectPtr sceneobject)
{
	// Claim ownership if there is none.
	if (sceneobject->GetOwningPlayer() == NO_PLAYER && (IsSinglePlayer() || IsHost()))
		sceneobject->SetOwningPlayer(m_player);

	// Execute scripts.
	auto object_class = sceneobject->GetClass();
	bool server_script = IsHost() || IsSinglePlayer();
	bool client_script = true;
	if (server_script)
	{
		Script->RunScript("sceneobject_sv_" + std::to_string(sceneobject->ID) + "_c_" + object_class->GetName(), object_class->ServerScript, sceneobject);
		Script->RunScript("sceneobject_sv_" + std::to_string(sceneobject->ID), sceneobject->ServerScript, sceneobject);
	}
	if (client_script)
	{
		Script->RunScript("sceneobject_cl_" + std::to_string(sceneobject->ID) + "_c_" + object_class->GetName(), object_class->ClientScript, sceneobject);
		Script->RunScript("sceneobject_cl" + std::to_string(sceneobject->ID), sceneobject->ClientScript, sceneobject);
	}

	sceneobject->OnCreated.RunAll();
	sceneobject->OnCreatePhysics.RunAll();

	if (m_player != nullptr)
		m_player->FollowedSceneObjects.insert(sceneobject->ID);

	sceneobject->PhysicsChanged = false;
	sceneobject->Initialized = true;

	return sceneobject;
}

bool Server::DeleteSceneObject(SceneObjectID id)
{
	auto so = GetSceneObjectById(id);
	return DeleteSceneObject(so);
}

bool Server::DeleteSceneObject(SceneObjectPtr sceneobject)
{
	if (sceneobject == nullptr)
		return false;
	if (!IsHost() && !sceneobject->IsLocal())
		return false;
	if (sceneobject->GetOwningPlayer() != m_player->GetPlayerId())
		return false;

	// Unfollow the scene object.
	m_player->FollowedSceneObjects.erase(sceneobject->ID);

	// Inform any clients that the scene object is being removed.
	if (OnSceneObjectRemove != nullptr)
		OnSceneObjectRemove(sceneobject);

	// Broadcast the removal.
	_sendDeleteSceneObjectToPlayers(*this, sceneobject, m_player);

	SCRIPT_THEM_ERASE(sceneobject);

	return true;
}

size_t Server::DeletePlayerOwnedSceneObjects(PlayerPtr player)
{
	size_t count = 0;

	// Only allow the host to remove other player's owned scene objects.
	if (!IsHost() && m_player != player)
		return count;

	for (auto& [name, scene] : m_scenes)
	{
		std::unordered_set<SceneObjectID> to_delete;

		auto& graph = scene->GetGraph();
		for (auto it = std::begin(graph); it != std::end(graph); ++it)
		{
			auto& sceneobject = it->second;
			if (sceneobject->GetOwningPlayer() == player->GetPlayerId())
			{
				// Unfollow the scene object.
				m_player->FollowedSceneObjects.erase(sceneobject->ID);

				// Inform any clients that the scene object is being removed.
				if (OnSceneObjectRemove != nullptr)
					OnSceneObjectRemove(sceneobject);

				// Broadcast the removal.
				_sendDeleteSceneObjectToPlayers(*this, sceneobject, m_player);

				SCRIPT_THEM_ERASE(sceneobject);

				++count;
				to_delete.insert(sceneobject->ID);
			}
		}

		for (auto& id : to_delete)
			graph.erase(id);
	}

	return count;
}

bool Server::SwitchSceneObjectScene(SceneObjectPtr sceneobject, scene::ScenePtr scene)
{
	bool switch_scene = !sceneobject->IsLocal();
	switch_scene |= sceneobject->IsLocal() && sceneobject->GetCurrentScene().expired();

	// If the scene object hasn't been added yet, do it now.
	if (!sceneobject->Initialized)
		AddSceneObject(sceneobject);

	if (!switch_scene) return false;
	if (!sceneobject->IsOwnedBy(m_player))
		return false;

	// Tell the old scene we have left.
	if (auto old_scene = sceneobject->GetCurrentScene().lock(); old_scene)
	{
		scene->RemoveObject(sceneobject);
		_sendDeleteSceneObjectToPlayers(*this, old_scene, sceneobject, m_player);
	}

	if (scene->AddObject(sceneobject))
	{
		// Inform the client we added a scene object.
		if (OnSceneObjectAdd != nullptr)
			OnSceneObjectAdd(sceneobject);

		// Send to players in range.
		_sendNewSceneObjectToPlayer(*this, m_player, sceneobject);
	}

	// Move attached scene objects too.
	for (const auto& attached : sceneobject->AttachedObjects())
	{
		if (auto attached_object = attached.lock(); attached_object)
			SwitchSceneObjectScene(attached_object, scene);
	}

	return true;
}

bool Server::SwitchSceneObjectOwnership(SceneObjectPtr sceneobject, PlayerPtr player)
{
	// Non-replicated objects can't be owned.
	if (!sceneobject->ReplicateChanges)
		return false;

	// Check for ownership of this scene object before we give it away.
	// Host can always give away ownership.
	auto old_player = sceneobject->GetOwningPlayer();
	if (!IsHost() && old_player != m_player->GetPlayerId())
		return false;

	uint16_t new_player_id = (player ? player->GetPlayerId() : 0);
	sceneobject->SetOwningPlayer(player);
	sceneobject->OnOwnershipChange.RunAll(player, old_player);

	// If the player isn't following this scene object, send it over first.
	if (!player->FollowedSceneObjects.contains(sceneobject->ID))
	{
		if (_sendNewSceneObjectToPlayer(*this, player, sceneobject))
			log::PrintLine(log::game, "-> Sending scene object {} via ownership change.", sceneobject->ID);

		player->FollowedSceneObjects.insert(sceneobject->ID);
	}

	// Switch ownership.
	auto scenew = sceneobject->GetCurrentScene();
	if (auto scene = scenew.lock())
	{
		packet::SceneObjectOwnership packet;
		packet.set_sceneobject_id(sceneobject->ID);
		packet.set_old_player_id(old_player);
		packet.set_new_player_id(new_player_id);

		BroadcastToScene(scene, PACKETID(Packets::SCENEOBJECTOWNERSHIP), network::Channel::RELIABLE, packet);
	}

	return true;
}

void Server::RequestSceneObjectChunkData(SceneObjectPtr sceneobject, uint32_t chunk_idx)
{
	if (IsHost() || sceneobject->GetType() != SceneObjectType::TMX)
		return;

	packet::SceneObjectRequestChunkData packet{};
	packet.set_id(sceneobject->ID);
	packet.set_index(chunk_idx);
	
	log::PrintLine(log::game, "-> Requesting chunk {} data for scene object {}.", chunk_idx, sceneobject->ID);
	Send(PACKETID(Packets::SCENEOBJECTREQUESTCHUNKDATA), network::Channel::RELIABLE, packet);
}

///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ObjectClass> Server::DeleteObjectClass(const std::string& name)
{
	// Only allow host to delete object classes.  Dangerous!
	if (!IsHost())
		return nullptr;

	auto iter = m_object_classes.find(name);
	if (iter == m_object_classes.end())
		return nullptr;

	auto& c = iter->second;
	m_object_classes.erase(iter);

	// Broadcast to players if we are the host.
	packet::ClassDelete packet;
	packet.set_name(name);

	Broadcast(PACKETID(Packets::CLASSDELETE), network::Channel::RELIABLE, packet);

	// TODO: Wipe out all script functions bound to this module.

	return c;
}

///////////////////////////////////////////////////////////////////////////////

bool Server::AddTileset(const std::string& name, scene::TilesetPtr tileset)
{
	auto it = m_tilesets.find(name);
	if (it != std::end(m_tilesets))
		return false;

	m_tilesets[name] = tileset;

	if (log::game.not_indented())
		log::Print(log::game, ":: Adding tileset: ");

	log::PrintLine(log::game, name);

	// Broadcast to players if we are the host.
	if (IsHost())
		Broadcast(PACKETID(Packets::SCENETILESETADD), network::Channel::RELIABLE, network::constructTilesetAddPacket(tileset));

	return true;
}

///////////////////////////////////////////////////////////////////////////////

int Server::SendEvent(scene::ScenePtr scene, PlayerPtr player, const std::string& name, const std::string& data)
{
	packet::SendEvent packet;
	packet.set_sender(player ? player->GetPlayerId() : 0);
	packet.set_name(name);
	packet.set_data(data);

	if (scene)
	{
		packet.set_scene(scene->GetName());
	}

	// Process the event.
	int count = ProcessSendEvent(scene, player, name, data);

	// Send the packet.
	if (scene)
	{
		log::PrintLine(log::game, "-> Sending level event '{}' to scene '{}' with data: {}", name, scene->GetName(), data);
		BroadcastToScene(scene, network::PACKETID(Packets::SENDEVENT), network::Channel::RELIABLE, packet, { m_player });
	}
	else
	{
		log::PrintLine(log::game, "-> Sending server event '{}' with data: {}", name, data);
		Broadcast(network::PACKETID(Packets::SENDEVENT), network::Channel::RELIABLE, packet);
	}

	return count;
}

int Server::SendEvent(scene::ScenePtr scene, PlayerPtr player, const std::string& name, const std::string& data, Vector2df origin, float radius)
{
	if (!scene) [[unlikely]]
		return 0;

	packet::SendEvent packet;
	packet.set_sender(player ? player->GetPlayerId() : 0);
	packet.set_scene(scene->GetName());
	packet.set_name(name);
	packet.set_data(data);
	packet.set_x(origin.x);
	packet.set_y(origin.y);
	packet.set_radius(radius);

	// Process the event.
	int count = ProcessSendEvent(scene, player, name, data, origin, radius);

	log::PrintLine(log::game, "-> [{}] Event '{}' ({:.2f}, {:.2f})-{}: {} hits, data: {}", scene->GetName(), name, origin.x, origin.y, radius, count, data);

	// Send the packet.
	SendToScene(scene, origin, network::PACKETID(Packets::SENDEVENT), network::Channel::RELIABLE, packet, { m_player });

	return count;
}

int Server::ProcessSendEvent(scene::ScenePtr scene, PlayerPtr player, const std::string& name, const std::string& data) const
{
	// Global events.
	OnServerEvent.RunAll(scene, player, name, data);
	return 0;
}

int Server::ProcessSendEvent(scene::ScenePtr scene, PlayerPtr player, const std::string& name, const std::string& data, Vector2df origin, float radius)
{
	if (!scene) [[unlikely]]
		return 0;

	// Get objects in range.
	auto targets = scene->FindObjectsByCollisionAndBoundInRange(origin, radius, [](SceneObjectPtr& so) { return !so->IgnoresEvents; });

	// Run events on each one.
	std::ranges::for_each(targets, [&](const auto& target) { target->OnEvent.RunAll(player, name, data, origin, radius); });

	return static_cast<int>(targets.size());
}

///////////////////////////////////////////////////////////////////////////////

SceneObjectPtr Server::GetSceneObjectById(SceneObjectID id)
{
	for (auto& [key, scene] : m_scenes)
	{
		if (auto so = scene->FindObject(id))
			return so;
	}

	return {};
}

/////////////////////////////

void Server::Send(const uint16_t packet_id, const network::Channel channel)
{
	m_network.Send(packet_id, channel);
}

void Server::Send(const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message)
{
	m_network.Send(packet_id, channel, message);
}

void Server::Send(const uint16_t player_id, const uint16_t packet_id, const network::Channel channel)
{
	m_network.Send(player_id, packet_id, channel);
}

void Server::Send(const uint16_t player_id, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message)
{
	m_network.Send(player_id, packet_id, channel, message);
}

void Server::Broadcast(const uint16_t packet_id, const network::Channel channel)
{
	if (IsSinglePlayer() || !IsHost())
		m_network.Send(packet_id, channel);
	else
		m_network.Broadcast(packet_id, channel);
}

void Server::Broadcast(const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message)
{
	if (IsSinglePlayer() || !IsHost())
		m_network.Send(packet_id, channel, message);
	else
		m_network.Broadcast(packet_id, channel, message);
}

std::vector<server::PlayerPtr> Server::SendToScene(const scene::ScenePtr scene, const Vector2df location, uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(packet_id, channel);
		return {};
	}
	else
		return m_network.SendToScene(scene, location, packet_id, channel, exclude);
}

std::vector<server::PlayerPtr> Server::SendToScene(const scene::ScenePtr scene, const Vector2df location, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(packet_id, channel, message);
		return {};
	}
	else
		return m_network.SendToScene(scene, location, packet_id, channel, message, exclude);
}

int Server::BroadcastToScene(const scene::ScenePtr scene, const uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(packet_id, channel);
		return 1;
	}
	else
		return m_network.BroadcastToScene(scene, packet_id, channel, exclude);
}

int Server::BroadcastToScene(const scene::ScenePtr scene, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(packet_id, channel, message);
		return 1;
	}
	else
		return m_network.BroadcastToScene(scene, packet_id, channel, message, exclude);
}

/////////////////////////////

void Server::network_connect(const uint32_t id)
{
	auto player_id = GetNextPlayerID();
	m_network.MapPlayerToPeer(player_id, id);
	log::PrintLine(log::game, "<- Connection from player {} ({}).", player_id, id);

	auto player = std::make_shared<Player>(player_id);
	player->BindServer(this);
	m_player_list[player_id] = player;
}

void Server::network_disconnect(const PlayerID player_id)
{
	auto player = GetPlayerById(player_id);
	if (!player) return;

	// Erase the player from the player list so packets don't get sent to them.
	m_player_list.erase(player_id);
	m_player_ids.FreeId(player_id);

	// Call the OnPlayerLeave script function.
	OnPlayerLeave.RunAll(player);

	// Save account.
	if (IsSinglePlayer() || IsHost())
		player->Account.Save();

	// TODO: Send disconnection packet to peers.
	log::PrintLine(log::game, "<- Disconnection from player {}.", player_id);

	SCRIPT_THEM_ERASE(player);
}

void Server::network_login(const PlayerID player_id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	if (packet_id != PACKETID(Packets::LOGIN))
		return;

	auto player = GetPlayerById(player_id);
	log::PrintLine(log::game, "<- Received login packet from player {}.", player_id);

	packet::Login packet;
	packet.ParseFromArray(packet_data, static_cast<int>(packet_length));
	const auto& method = packet.method();
	const auto& account = packet.account();
	const auto& passwordhash = packet.passwordhash();
	const auto& nickname = packet.nickname();
	const auto& type = packet.type();
	const auto& version = packet.version();

	// If this is a peer-to-peer connection, allow the player through.
	if (method == packet::Login_Method_PEER2PEER)
	{
		ProcessPlayerLogin(player_id, nickname);
		return;
	}

	// TODO: Properly handle account verification for servers that use it.

	// Login failure.
	if (false)
	{
		packet::LoginStatus login_status;
		login_status.set_player_id(player_id);
		login_status.set_success(false);
		login_status.set_message("Invalid username or password.");
		Send(player_id, PACKETID(Packets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
		log::PrintLine(log::game, "-> Sending login status - failure.");

		m_network.DisconnectPeer(player_id);
		return;
	}

	// Log in the player.
	ProcessPlayerLogin(player_id, account);
}

} // end namespace tdrp::server
