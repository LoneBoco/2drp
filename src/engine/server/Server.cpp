#include <chrono>
#include <iostream>
#include <unordered_set>

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

using tdrp::network::PACKETID;
using tdrp::network::Packets;

namespace tdrp::server
{

inline bool _bypass(Server* server, const uint16_t peer_id)
{
	bool bypass = server->IsSinglePlayer();
	if (server->IsHost() && peer_id == network::HOSTID)
		bypass = true;
	return bypass;
}

std::vector<uint8_t> _serializeMessageToVector(const google::protobuf::Message& message)
{
	size_t message_size = message.ByteSizeLong();

	std::vector<uint8_t> result(message_size);
	message.SerializeToArray(result.data(), static_cast<int>(message_size));

	return result;
}

/////////////////////////////

Server::Server()
	: m_connecting(false), m_server_type(ServerType::HOST), m_server_flags(0), m_sceneobject_counter(0), m_server_name("PEER"), m_max_players(8)
{
	auto script_manager = BabyDI::Get<script::ScriptManager>();
	Script = script_manager->CreateScriptInstance("Server");
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
}

Server::~Server()
{
	Shutdown();
}

bool Server::Initialize(const std::string& package_name, const ServerType type, const uint16_t flags)
{
	network::Network::Shutdown();
	if (!network::Network::Startup())
		return false;

	m_server_type = type;
	m_server_flags = flags;

	log::PrintLine(":: Initializing server.");

	// Load the package.
	auto[load_success, package] = Loader::LoadPackageIntoServer(*this, package_name);
	m_package = package;

	// Sanity check for starting scene.
	if (package->GetStartingScene().empty())
		throw std::runtime_error("!! No starting scene for package!");

	// Load everything from the package into the server.
	if (HASFLAG(m_server_flags, ServerFlags::PRELOAD_EVERYTHING))
	{
		log::PrintLine(":: Loading all scenes in package.");

		// Load all scenes.
		for (auto& d : filesystem::directory_iterator{ package->GetBasePath() / "levels" })
		{
			if (!filesystem::is_directory(d.status()))
				continue;

			auto scene = Loader::CreateScene(*this, d);
			if (scene != nullptr)
				m_scenes.insert(std::make_pair(d.path().filename().string(), scene));
		}
	}
	// Load only the starting scene into the server.
	else
	{
		log::PrintLine(":: Loading the starting scene: {}.", package->GetStartingScene());

		// Load our starting scene.
		auto scene = Loader::CreateScene(*this, package->GetBasePath() / "levels" / package->GetStartingScene());
		if (scene != nullptr)
			m_scenes.insert(std::make_pair(package->GetStartingScene(), scene));
	}

	// Load server script.
	if (IsHost())
	{
		log::PrintLine(":: Loading server control script.");
		Script->RunScript("servercontrol", m_server_control_script, this);
	}

	// Run our started script.
	OnStarted.RunAll();

	return true;
}

bool Server::HostDedicated(const uint16_t port, const size_t peers)
{
	m_server_type = ServerType::DEDICATED;
	UNSETFLAG(m_server_flags, ServerFlags::SINGLEPLAYER);

	log::PrintLine(":: Hosting on port {} for {} peers.", port, peers);

	m_network.Initialize(peers, port);
	return true;
}

bool Server::Host(const uint16_t port, const size_t peers)
{
	m_server_type = ServerType::HOST;
	UNSETFLAG(m_server_flags, ServerFlags::SINGLEPLAYER);

	log::PrintLine(":: Hosting on port {} for {} peers.", port, peers);

	// Initialize our network and connect.
	m_network.Initialize(peers, port);
	network_connect(0);

	// Log the player in.
	SetPlayerNumber(0);
	ProcessPlayerLogin(0, "host");
	ProcessPlayerJoin(0);

	return true;
}

bool Server::Connect(const std::string& hostname, const uint16_t port)
{
	m_server_type = ServerType::GUEST;
	m_server_flags = 0;

	log::PrintLine(":: Connecting to {} on port {}.", hostname, port);

	m_network.Initialize();
	m_connecting = true;
	m_connecting_future = m_network.Connect(hostname, port);
	return true;
}

bool Server::SinglePlayer()
{
	SETFLAG(m_server_flags, ServerFlags::SINGLEPLAYER);

	log::PrintLine(":: Starting as single player server.");

	network_connect(0);

	// Log the player in.
	SetPlayerNumber(0);
	ProcessPlayerLogin(0, "singleplayer");
	ProcessPlayerJoin(0);

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
				log::PrintLine(":: Connection was successful, sending login packet.");

				packet::Login packet;
				packet.set_method(packet::Login_Method_PEER2PEER);
				/*
				packet.set_account("test");
				packet.set_passwordhash("testpass");
				packet.set_type(0);
				packet.set_version("first");
				*/

				Send(network::HOSTID, PACKETID(Packets::LOGIN), network::Channel::RELIABLE, packet);
			}
			else
			{
				log::PrintLine("!! Connection failed.");
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
				if (p->FollowedSceneObjects.contains(item->ID))
					return;

				new_objects.insert(item->ID);

				log::PrintLine("-> Sending scene object {} via range to player {}.", item->ID, player->GetPlayerId());

				auto packet = network::constructSceneObjectPacket(item);
				Send(player->GetPlayerId(), PACKETID(Packets::SCENEOBJECTNEW), network::Channel::RELIABLE, packet);
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
			if (IsHost() && object->GetOwningPlayer().expired())
				SwitchSceneObjectOwnership(object, m_player);

			// If the scene object is not replicated across the network, don't send anything.
			if (!object->Replicated)
				continue;

			// If we are in single player mode, there is no need to send anything.
			if (IsSinglePlayer())
				continue;

			// If we don't own this scene object, don't send anything.
			if (auto op = object->GetOwningPlayer().lock(); op != m_player)
				continue;

			// Send the packet to all players in the scene.
			packet.set_id(object->ID);
			packet.set_replicated(object->Replicated);
			packet.set_attached_to(object->GetAttachedId());
			auto location = object->GetPosition();
			SendToScene(scene, location, PACKETID(Packets::SCENEOBJECTCHANGE), network::Channel::RELIABLE, packet, { m_player });

			// Send over any physics changes to the scene object.
			// This is changes in collision mesh or simulated body type, not velocities or anything.
			if (object->PhysicsChanged)
			{
				object->PhysicsChanged = false;
				if (object->PhysicsBody.has_value())
				{
					packet::SceneObjectCollision packet;
					packet.set_id(object->ID);

					network::writeCollisionToPacket(object->PhysicsBody.value(), scene, packet);

					auto location = object->GetPosition();
					SendToScene(scene, location, PACKETID(Packets::SCENEOBJECTCOLLISION), network::Channel::RELIABLE, packet, { m_player });
				}
			}
		}
	}

	if (!IsSinglePlayer() && !m_connecting)
		m_network.Update();

	FileSystem.Update();
}

void Server::ProcessPlayerJoin(const uint16_t player_id)
{
	auto player = GetPlayerById(player_id);

	// Switch to the starting scene.
	auto scene = GetScene(GetPackage()->GetStartingScene());
	player->SwitchScene(scene);

	// Call the OnPlayerJoin script function.
	OnPlayerJoin.RunAll(player);
}

void Server::ProcessPlayerLogin(const uint16_t player_id, const std::string& account)
{
	// Load account.
	log::PrintLine(":: Loading account {}.", account);
	auto player = GetPlayerById(player_id);
	player->Account.Load(account);

	// Tell the client the login was a success.
	packet::LoginStatus login_status;
	login_status.set_player_id(player_id);
	login_status.set_success(true);
	Send(player_id, PACKETID(Packets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
	log::PrintLine("-> Sending login allowed.");

	// Send client control script.
	log::PrintLine("-> Sending client control script to player {}.", player_id);
	packet::ClientControlScript client_control;
	client_control.set_script(m_client_control_script);
	Send(player_id, PACKETID(Packets::CLIENTCONTROLSCRIPT), network::Channel::RELIABLE, client_control);

	// Send classes.
	for (const auto& [name, oc] : m_object_classes)
	{
		auto packet = network::constructClassAddPacket(oc);
		Send(player_id, PACKETID(Packets::CLASSADD), network::Channel::RELIABLE, packet);
	}

	// Send client scripts.
	for (const auto& name : player->Account.ClientScripts)
	{
		AddPlayerClientScript(name, player);
	}

	// Send owned items.
	for (const auto& [itemId, item] : player->Account.Items)
	{
		// Send item definitions.
		SendItemDefinition(m_player, item->ItemBaseID);

		// Send the item.
		packet::ItemAdd packetAdd;
		packetAdd.set_id(item->ID);
		packetAdd.set_baseid(item->ItemBaseID);
		packetAdd.set_type(static_cast<packet::ItemType>(item->Type));

		if (auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(item); stackable)
			packetAdd.set_stackable_count(stackable->Count);

		if (auto variant = std::dynamic_pointer_cast<item::ItemVariant>(item); variant)
			network::assignAllAttributesToPacket(packetAdd, variant->VariantAttributes, &packet::ItemAdd::add_variant_attributes);

		log::PrintLine("-> Sending weapon {} (base {}, type {}) to player {}.", item->ID, item->ItemBaseID, static_cast<uint8_t>(item->Type), player->GetPlayerId());
		Send(player->GetPlayerId(), network::PACKETID(network::Packets::ITEMADD), network::Channel::RELIABLE, packetAdd);
	}

	// Send server info.
	// Keep this last.  The client will start the load after this packet.
	packet::ServerInfo server_info;
	server_info.set_uniqueid(m_unique_id);
	server_info.set_package(m_package->GetName());
	server_info.set_loadingscene(m_package->GetLoadingScene());
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
	Send(player_id, PACKETID(Packets::SERVERINFO), network::Channel::RELIABLE, server_info);
}

void Server::SetPlayerNumber(const uint16_t player_number)
{
	log::PrintLine(":: Player number {}.", player_number);

	if (IsSinglePlayer() || IsHost())
	{
		auto player = GetPlayerById(player_number);
		m_player = player;
	}
	else
	{
		auto player = std::make_shared<Player>(player_number);
		player->BindServer(this);
		m_player_list[player_number] = player;
		m_player = player;
	}
}

///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ObjectClass> Server::GetObjectClass(const std::string& name)
{
	auto iter = m_object_classes.find(name);
	if (iter == m_object_classes.end())
		iter = m_object_classes.find("blank");

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
		if (player->FollowedSceneObjects.contains(sceneobject->ID))
			continue;

		player->FollowedSceneObjects.insert(sceneobject->ID);

		if (!(IsHost() && m_player == player && old_scene.expired()))
			log::PrintLine("-> Sending scene object {} via scene switch.", sceneobject->ID);

		auto object = network::constructSceneObjectPacket(sceneobject);
		Send(player->GetPlayerId(), network::PACKETID(network::Packets::SCENEOBJECTNEW), network::Channel::RELIABLE, object);

		// If there is no owning player, take ownership.
		if (sceneobject->GetOwningPlayer().expired())
			SwitchSceneObjectOwnership(sceneobject, player);
	}

	return true;
}

/*
bool Server::SwitchPlayerControlledSceneObject(std::shared_ptr<server::Player>& player, std::shared_ptr<SceneObject>& new_scene_object)
{
	if (player == nullptr || new_scene_object == nullptr)
		return false;

	auto old_so = player->SwitchControlledSceneObject(new_scene_object);

	auto old_id = 0;
	if (auto so = old_so.lock())
		old_id = so->ID;

	// If we aren't following this scene object we now have control over, send it over now.
	if (!player->FollowedSceneObjects.contains(new_scene_object->ID))
	{
		player->FollowedSceneObjects.insert(new_scene_object->ID);
		auto packet_newso = network::constructSceneObjectPacket(new_scene_object);
		m_network.Send(player->GetPlayerId(), PACKETID(network::Packets::SCENEOBJECTNEW), network::Channel::RELIABLE, packet_newso);
	}

	packet::SceneObjectControl packet;
	packet.set_old_id(old_id);
	packet.set_new_id(new_scene_object->ID);
	m_network.Send(player->GetPlayerId(), network::PACKETID(network::Packets::SCENEOBJECTCONTROL), network::Channel::RELIABLE, packet);

	if (!IsSinglePlayer())
		new_scene_object->OnPlayerGainedControl.RunAll(player);

	return true;
}
*/

///////////////////////////////////////////////////////////////////////////////

void Server::LoadClientScript(const filesystem::path& file)
{
	auto name = file.stem().string();

	std::string script;
	auto f = FileSystem.GetFile(file);
	if (f && f->Opened())
		script = f->ReadAsString();

	LoadClientScript(name, script);
}

void Server::LoadClientScript(const std::string& name, const std::string& script)
{
	auto it = m_client_scripts.find(name);
	if (it == std::end(m_client_scripts))
	{
		log::PrintLine(":: Adding client script \"{}\".", name);
		m_client_scripts[name] = script;
	}
	else
	{
		log::PrintLine(":: Replacing client script \"{}\".", name);
		m_client_scripts[name] = script;

		// Send the changes to all players who have this script.
		if (IsHost())
		{
			for (auto& player : m_player_list)
			{
				if (player.second->Account.ClientScripts.contains(name))
				{
					packet::ClientScriptAdd packet;
					packet.set_name(name);
					packet.set_script(script);

					Send(player.first, PACKETID(Packets::CLIENTSCRIPTADD), network::Channel::RELIABLE, packet);
				}
			}
		}
	}
}

void Server::EraseClientScript(const std::string& name)
{
	auto it = m_client_scripts.find(name);
	if (it == std::end(m_client_scripts))
		return;

	log::PrintLine(":: Erasing client script \"{}\".", name);
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

	player->Account.AddClientScript(name);

	if (IsHost())
	{
		const auto& script = it->second;

		packet::ClientScriptAdd packet;
		packet.set_name(name);
		packet.set_script(script);

		log::PrintLine("-> Sending client script \"{}\" to player {}.", name, player->GetPlayerId());
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

		log::PrintLine("-> Removing client script \"{}\" from player {}.", name, player->GetPlayerId());
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
	if (success) log::PrintLine(":: Loaded item {}.{}.", baseID, name);
	else log::PrintLine("!! Failed to add item {}.{}. Item with the same ID already exists.", baseID, name);

	return success;
}

void Server::SendItemDefinition(server::PlayerPtr player, ItemID baseId)
{
	// Don't send the item definition if the player already knows it.
	if (player->KnowsItemDefinition(baseId))
		return;

	if (auto baseItem = GetItemDefinition(baseId); baseItem)
	{
		log::PrintLine("-> Sending item definition {}.{} to player {}.", baseId, baseItem->Name, player->GetPlayerId());
		player->AddKnownItemDefinition(baseId);

		// Only send item definitions if we are the host.
		if (IsHost() && player != m_player)
		{
			packet::ItemDefinition packetDef;
			packetDef.set_baseid(baseItem->BaseID);
			packetDef.set_name(baseItem->Name);
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
	if (IsGuest() || player == nullptr || count == 0)
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

	auto host_script = [this](item::ItemInstancePtr& item, const PlayerPtr& player, size_t count = 1, bool is_new = true)
	{
		if (m_player != player) return;
		if (is_new)
		{
			Script->RunScript(std::format("item_{}", item->ID), item->ItemBase->ClientScript, item);
			//item->LuaEnvironment->set("Server", this);
			item->OnCreated.RunAll();
		}
		item->OnAdded.RunAll(count);
	};

	auto send_item = [this, &host_script](item::ItemInstancePtr& item, const PlayerPtr& player, size_t count = 1, bool is_new = true)
	{
		host_script(item, player, count, is_new);
		if (m_player == player) return;
		
		// Try to send the item definition.
		SendItemDefinition(player, item->ItemBaseID);

		// Send the item.
		packet::ItemAdd packetAdd;
		packetAdd.set_id(item->ID);
		packetAdd.set_baseid(item->ItemBaseID);
		packetAdd.set_type(static_cast<packet::ItemType>(item->Type));

		if (auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(item); stackable)
			packetAdd.set_stackable_count(count);

		if (auto variant = std::dynamic_pointer_cast<item::ItemVariant>(item); variant)
			network::assignAllAttributesToPacket(packetAdd, variant->VariantAttributes, &packet::ItemAdd::add_variant_attributes);

		Send(player->GetPlayerId(), network::PACKETID(network::Packets::ITEMADD), network::Channel::RELIABLE, packetAdd);
	};

	// Create the item and bind it to the player's account.
	// Item creation for clients happen in the ITEMADD packet.
	// We have to handle scripts for the host here as the ITEMADD packet will be ignored.

	switch (type)
	{
		// Single items that only have one instance.
		case item::ItemType::SINGLE:
		{
			auto existing = std::ranges::find_if(player->Account.Items, match_single);
			if (existing != std::ranges::end(player->Account.Items))
				return existing->second;

			auto instance = std::make_shared<item::ItemInstance>(next_id(), baseId);
			instance->ItemBase = GetItemDefinition(baseId);

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
				instance->ItemBase = GetItemDefinition(baseId);

				send_item(instance, player, count, false);
				return instance;
			}
			else
			{
				auto stackable = std::make_shared<item::ItemStackable>(next_id(), baseId);
				auto instance = std::dynamic_pointer_cast<item::ItemInstance>(stackable);
				stackable->Count = count;
				stackable->ItemBase = GetItemDefinition(baseId);

				player->Account.AddItem(instance);
				send_item(instance, player, count);
				return instance;
			}
			break;
		}

		case item::ItemType::VARIANT:
		{
			auto variant = std::make_shared<item::ItemVariant>(next_id(), baseId);
			auto instance = std::dynamic_pointer_cast<item::ItemInstance>(variant);
			instance->ItemBase = GetItemDefinition(baseId);

			player->Account.AddItem(instance);
			send_item(instance, player);
			return instance;
		}
	}

	return nullptr;
}

item::ItemInstancePtr Server::RemoveItemFromPlayer(server::PlayerPtr player, ItemID id, size_t count)
{
	if (IsGuest()) return nullptr;

	if (player == nullptr) return nullptr;
	auto iter = player->Account.Items.find(id);
	if (iter == std::end(player->Account.Items))
		return nullptr;

	auto& item = iter->second;
	if (item->Type == item::ItemType::SINGLE || item->Type == item::ItemType::VARIANT)
		player->Account.RemoveItem(item);
	else if (auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(item); stackable)
	{
		count = std::min(stackable->Count, count);
		stackable->Count -= count;
		if (stackable->Count == 0)
			player->Account.RemoveItem(item);
	}

	// Host item add, just run the script and return.
	if (player == m_player)
	{
		item->OnRemoved.RunAll(count);
		return item;
	}

	packet::ItemDelete packet;
	packet.set_id(item->ID);
	packet.set_count(count);

	Send(player->GetPlayerId(), PACKETID(network::Packets::ITEMDELETE), network::Channel::RELIABLE, packet);
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

SceneObjectPtr Server::CreateSceneObject(SceneObjectType type, const std::string& object_class)
{
	// TODO: Let server allow client side creation.
	if (!IsHost())
		return nullptr;

	// Get our object class.
	auto oc = GetObjectClass(object_class);
	if (oc == nullptr)
		return nullptr;

	// Retrieve the new ID for our scene object.
	// Set the global flag.
	auto id = GetNextSceneObjectID() | GlobalSceneObjectIDFlag;

	// Create the scene object.
	SceneObjectPtr so = nullptr;
	switch (type)
	{
	case SceneObjectType::DEFAULT:
		so = std::make_shared<tdrp::SceneObject>(oc, id);
		break;
	case SceneObjectType::STATIC:
		so = std::make_shared<tdrp::StaticSceneObject>(oc, id);
		break;
	case SceneObjectType::ANIMATED:
		so = std::make_shared<tdrp::AnimatedSceneObject>(oc, id);
		break;
	case SceneObjectType::TILEMAP:
		so = std::make_shared<tdrp::TiledSceneObject>(oc, id);
		break;
	case SceneObjectType::TMX:
		so = std::make_shared<tdrp::TMXSceneObject>(oc, id);
		break;
	case SceneObjectType::TEXT:
		so = std::make_shared<tdrp::TextSceneObject>(oc, id);
		break;
	default:
		log::PrintLine("!! CreateSceneObject requires a valid scene object type.");
		return nullptr;
	}

	// Claim ownership.
	if (m_player)
	{
		so->SetOwningPlayer(m_player);
		m_player->FollowedSceneObjects.insert(id);
	}

	// Handle the script.
	if (IsHost() && so && oc)
	{
		Script->RunScript("sceneobject_sv_" + std::to_string(id) + "_c_" + oc->GetName(), oc->ScriptServer, so);
		Script->RunScript("sceneobject_sv_" + std::to_string(id), so->ServerScript, so);
	}

	// Single player checks.
	if (IsSinglePlayer())
	{
		Script->RunScript("sceneobject_cl_" + std::to_string(id) + "_c_" + oc->GetName(), oc->ScriptClient, so);
		Script->RunScript("sceneobject_cl" + std::to_string(id), so->ClientScript, so);
	}

	return so;
}

SceneObjectPtr Server::CreateSceneObject(SceneObjectType type, const std::string& object_class, scene::ScenePtr scene)
{
	auto so = CreateSceneObject(type, object_class);

	// Add to the scene.
	scene->AddObject(so);

	// Follow the scene object if it is in our scene.
	if (scene == m_player->GetCurrentScene().lock())
		m_player->FollowedSceneObjects.insert(so->ID);

	// Send to players in range.
	auto packet = network::constructSceneObjectPacket(so);
	SendToScene(scene, so->GetPosition(), PACKETID(Packets::SCENEOBJECTNEW), network::Channel::RELIABLE, packet);

	// This is a new object so the physics get sent already.
	so->PhysicsChanged = false;

	return so;
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
	if (!sceneobject->IsGlobal() && !IsHost())
		return false;
	if (sceneobject->GetOwningPlayer().lock() != m_player)
		return false;

	packet::SceneObjectDelete packet;
	packet.set_id(sceneobject->ID);

	auto scene = sceneobject->GetCurrentScene().lock();
	if (!scene)
	{
		Broadcast(PACKETID(Packets::SCENEOBJECTDELETE), network::Channel::RELIABLE, packet);
	}
	else
	{
		scene->RemoveObject(sceneobject);
		BroadcastToScene(scene, PACKETID(Packets::SCENEOBJECTDELETE), network::Channel::RELIABLE, packet, { m_player });
	}

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
			if (sceneobject->GetOwningPlayer().lock() == player)
			{
				SCRIPT_THEM_ERASE(sceneobject);

				packet::SceneObjectDelete packet;
				packet.set_id(sceneobject->ID);
				BroadcastToScene(scene, PACKETID(Packets::SCENEOBJECTDELETE), network::Channel::RELIABLE, packet, { m_player });

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
	bool switch_scene = sceneobject->IsGlobal();
	switch_scene |= !sceneobject->IsGlobal() && sceneobject->GetCurrentScene().expired();

	if (!switch_scene) return false;

	// Tell the old scene we have left.
	if (auto old_scene = sceneobject->GetCurrentScene().lock(); old_scene)
	{
		scene->RemoveObject(sceneobject);

		packet::SceneObjectDelete pdelete;
		pdelete.set_id(sceneobject->ID);
		SendToScene(old_scene, sceneobject->GetPosition(), PACKETID(Packets::SCENEOBJECTDELETE), network::Channel::RELIABLE, pdelete);
	}

	if (scene->AddObject(sceneobject))
	{
		// Send to players in range.
		auto packet = network::constructSceneObjectPacket(sceneobject);
		SendToScene(scene, sceneobject->GetPosition(), PACKETID(Packets::SCENEOBJECTNEW), network::Channel::RELIABLE, packet);
	}

	return true;
}

bool Server::SwitchSceneObjectOwnership(SceneObjectPtr sceneobject, PlayerPtr player)
{
	// Non-replicated objects can't be owned.
	if (!sceneobject->Replicated)
		return false;

	// Check for ownership of this scene object before we give it away.
	// Host can always give away ownership.
	auto old_player = sceneobject->GetOwningPlayer().lock();
	if (!IsHost() && old_player != m_player)
		return false;

	uint16_t old_player_id = 0;
	uint16_t new_player_id = 0;

	if (old_player) old_player_id = old_player->GetPlayerId();
	if (player) new_player_id = player->GetPlayerId();

	sceneobject->SetOwningPlayer(player);
	sceneobject->OnOwnershipChange.RunAll(player, old_player);

	// If the player isn't following this scene object, send it over first.
	if (!player->FollowedSceneObjects.contains(sceneobject->ID))
	{
		if (m_player != player)
			log::PrintLine("-> Sending scene object {} via ownership change.", sceneobject->ID);

		player->FollowedSceneObjects.insert(sceneobject->ID);
		auto packet_newso = network::constructSceneObjectPacket(sceneobject);
		Send(player->GetPlayerId(), PACKETID(network::Packets::SCENEOBJECTNEW), network::Channel::RELIABLE, packet_newso);
	}

	// Switch ownership.
	auto scenew = sceneobject->GetCurrentScene();
	if (auto scene = scenew.lock())
	{
		packet::SceneObjectOwnership packet;
		packet.set_sceneobject_id(sceneobject->ID);
		packet.set_old_player_id(old_player_id);
		packet.set_new_player_id(new_player_id);

		BroadcastToScene(scene, PACKETID(Packets::SCENEOBJECTOWNERSHIP), network::Channel::RELIABLE, packet);
	}

	return true;
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
		log::PrintLine("-> Sending level event '{}' to scene '{}' with data: {}", name, scene->GetName(), data);
		BroadcastToScene(scene, network::PACKETID(Packets::SENDEVENT), network::Channel::RELIABLE, packet, { m_player });
	}
	else
	{
		log::PrintLine("-> Sending server event '{}' with data: {}", name, data);
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

	log::PrintLine("-> Sending event '{}' to scene '{}' at ({}, {}):{} with data: {}", name, scene->GetName(), origin.x, origin.y, radius, data);
	log::PrintLine("   -- hits: {}", count);

	// Send the packet.
	SendToScene(scene, origin, network::PACKETID(Packets::SENDEVENT), network::Channel::RELIABLE, packet, { m_player });

	return count;
}

int Server::ProcessSendEvent(scene::ScenePtr scene, PlayerPtr player, const std::string& name, const std::string& data)
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

void Server::Send(const uint16_t peer_id, const uint16_t packet_id, const network::Channel channel)
{
	m_network.Send(peer_id, packet_id, channel);
}

void Server::Send(const uint16_t peer_id, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message)
{
	m_network.Send(peer_id, packet_id, channel, message);
}

void Server::Broadcast(const uint16_t packet_id, const network::Channel channel)
{
	if (IsSinglePlayer() || !IsHost())
		m_network.Send(network::HOSTID, packet_id, channel);
	else
		m_network.Broadcast(packet_id, channel);
}

void Server::Broadcast(const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message)
{
	if (IsSinglePlayer() || !IsHost())
		m_network.Send(network::HOSTID, packet_id, channel, message);
	else
		m_network.Broadcast(packet_id, channel, message);
}

std::vector<server::PlayerPtr> Server::SendToScene(const scene::ScenePtr scene, const Vector2df location, uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(network::HOSTID, packet_id, channel);
		return {};
	}
	else
		return m_network.SendToScene(scene, location, packet_id, channel, exclude);
}

std::vector<server::PlayerPtr> Server::SendToScene(const scene::ScenePtr scene, const Vector2df location, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(network::HOSTID, packet_id, channel, message);
		return {};
	}
	else
		return m_network.SendToScene(scene, location, packet_id, channel, message, exclude);
}

int Server::BroadcastToScene(const scene::ScenePtr scene, const uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(network::HOSTID, packet_id, channel);
		return 1;
	}
	else
		return m_network.BroadcastToScene(scene, packet_id, channel, exclude);
}

int Server::BroadcastToScene(const scene::ScenePtr scene, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(network::HOSTID, packet_id, channel, message);
		return 1;
	}
	else
		return m_network.BroadcastToScene(scene, packet_id, channel, message, exclude);
}

/////////////////////////////

void Server::network_connect(const uint16_t id)
{
	log::PrintLine("<- Connection from player {}.", id);
	auto player = std::make_shared<Player>(id);
	player->BindServer(this);
	m_player_list[id] = player;
}

void Server::network_disconnect(const uint16_t id)
{
	auto player = GetPlayerById(id);
	if (!player) return;

	// Erase the player from the player list so packets don't get sent to them.
	m_player_list.erase(id);

	// Call the OnPlayerLeave script function.
	OnPlayerLeave.RunAll(player);

	// Save account.
	if (IsSinglePlayer() || IsHost())
		player->Account.Save();

	// TODO: Send disconnection packet to peers.
	log::PrintLine("<- Disconnection from player {}.", id);

	SCRIPT_THEM_ERASE(player);
}

void Server::network_login(const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	if (packet_id != PACKETID(Packets::LOGIN))
		return;

	auto player = GetPlayerById(id);
	log::PrintLine("<- Received login packet from player {}.", id);

	packet::Login packet;
	packet.ParseFromArray(packet_data, static_cast<int>(packet_length));
	const auto& method = packet.method();
	const auto& account = packet.account();
	const auto& passwordhash = packet.passwordhash();
	const auto& type = packet.type();
	const auto& version = packet.version();

	// TODO: This is a temporary override while we work on the account system.
	std::string login_account = account;

	// TODO: Properly handle account verification for servers that use it.
	// Load the account.
	if (method == packet::Login_Method_DEDICATED)
	{
		login_account = "dedicated";
	}
	else
	{
		login_account = (id == network::HOSTID && IsHost()) ? "host" : "guest" + std::to_string(id);
	}

	// Login failure.
	if (false)
	{
		packet::LoginStatus login_status;
		login_status.set_player_id(id);
		login_status.set_success(false);
		login_status.set_message("Invalid username or password.");
		Send(id, PACKETID(Packets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
		log::PrintLine("-> Sending login status - failure.");

		m_network.DisconnectPeer(id);
		return;
	}

	// Log in the player.
	ProcessPlayerLogin(id, login_account);
}

} // end namespace tdrp::server
