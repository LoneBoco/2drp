#include <chrono>
#include <iostream>

#include "engine/os/Utils.h"

#include "engine/server/Server.h"
#include "engine/server/Account.h"
#include "engine/loader/PackageLoader.h"
#include "engine/loader/LevelLoader.h"
#include "engine/filesystem/File.h"
#include "engine/network/Packet.h"
#include "engine/network/PacketsInc.h"
#include "engine/network/PacketHandler.h"
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
	if (IsHost() || IsSinglePlayer())
	{
		network_disconnect(0);
	}

	network::Network::Shutdown();
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
	auto[load_success, package] = loader::PackageLoader::LoadIntoServer(*this, package_name);
	m_package = package;

	// Sanity check for starting scene.
	if (package->GetStartingScene().empty())
		throw std::runtime_error("!! No starting scene for package!");

	// Load everything from the package into the server.
	if (HASFLAG(m_server_flags, ServerFlags::PRELOAD_EVERYTHING))
	{
		log::PrintLine(":: Loading everything.");

		// Load all scenes.
		for (auto& d : filesystem::directory_iterator{ package->GetBasePath() / "levels" })
		{
			if (!filesystem::is_directory(d.status()))
				continue;

			auto scene = loader::LevelLoader::CreateScene(*this, d);
			if (scene != nullptr)
				m_scenes.insert(std::make_pair(d.path().filename().string(), scene));
		}
	}
	// Load only the starting scene into the server.
	else
	{
		log::PrintLine(":: Loading the starting scene: {}.", package->GetStartingScene());

		// Load our starting scene.
		auto scene = loader::LevelLoader::CreateScene(*this, package->GetBasePath() / "levels" / package->GetStartingScene());
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

	m_network.Initialize(peers, port);
	m_connecting = true;

	// Force a successful connection.
	std::promise<bool> connecting_promise;
	m_connecting_future = connecting_promise.get_future();
	connecting_promise.set_value(true);

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

	// Network.Initialize();

	network_connect(0);

	packet::Login packet;
	packet.set_method(packet::Login_Method_PEER2PEER);
	packet.set_account("singleplayer");
	Send(network::HOSTID, PACKETID(Packets::LOGIN), network::Channel::RELIABLE, packet);

	return true;
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

void Server::Update(chrono::clock::duration tick)
{
	auto tick_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick).count();

	// Run server update script.
	OnServerTick.RunAll(tick_in_ms);

	// Iterate through all the players and determine if any new scene objects should be sent.
	for (auto& [id, player] : m_player_list)
	{
		if (player == nullptr)
			continue;

		// Don't send to ourselves.
		if (player == m_player)
			continue;

		auto pso = player->GetCurrentControlledSceneObject().lock();
		auto scene = player->GetCurrentScene().lock();
		if (pso && scene)
		{
			// Get all scene objects nearby.
			auto nearby = scene->FindObjectsInRangeOf(pso->GetPosition(), scene->GetTransmissionDistance() * 1.5f);

			std::set<uint32_t> new_objects;

			// Find every one not known about.
			//for (auto& so : nearby | std::views)
			auto& p = player;
			std::for_each(std::begin(nearby), std::end(nearby), [&](const decltype(nearby)::value_type& item)
			{
				if (p->FollowedSceneObjects.contains(item->ID))
					return;

				new_objects.insert(item->ID);

				log::PrintLine("-> Sending scene object {} via range.", item->ID);

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
		for (auto& [id, object] : scene->m_graph)
		{
			// Run the server tick script on the object.
			object->OnUpdate.RunAll(tick_in_ms);

			// If we don't own this scene object, don't check dirty props.
			// Non-replicated objects skip this check.
			if ((object->GetOwningPlayer().expired() || object->GetOwningPlayer().lock() != m_player) && !object->NonReplicated)
			{
				// If the scene object isn't owned and we are the host, we'll take care of the events.
				if (!IsHost())
					continue;
			}

			// Check if we have dirty attributes.
			if (object->Properties.HasDirty() || object->Attributes.HasDirty())
			{
				// Create a packet that contains our updated attributes.
				auto packet = getPropsPacket<packet::SceneObjectChange>(*object);

				// Nothing is being sent anymore.  The event dispatch may have resolved the dirty status.
				if (packet.attributes_size() == 0 && packet.properties_size() == 0)
					continue;

				packet.set_id(id);
				packet.set_non_replicated(object->NonReplicated);
				packet.set_attached_to(object->GetAttachedId());

				if (!IsSinglePlayer() && !object->NonReplicated)
				{
					// log::PrintLine("-> Sending prop updates for scene object {}.", id);

					auto location = object->GetPosition();
					SendToScene(scene, location, PACKETID(Packets::SCENEOBJECTCHANGE), network::Channel::RELIABLE, packet, { m_player });
				}
			}
		}
	}

	if (!IsSinglePlayer() && !m_connecting)
		m_network.Update();

	FileSystem.Update();
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

std::shared_ptr<scene::Tileset> Server::GetTileset(const std::string& name)
{
	auto iter = m_tilesets.find(name);
	if (iter == m_tilesets.end())
		return nullptr;
	return iter->second;
}

std::shared_ptr<scene::Scene> Server::GetScene(const std::string& name)
{
	auto iter = m_scenes.find(name);
	if (iter == m_scenes.end())
		return nullptr;
	return iter->second;
}

///////////////////////////////////////////////////////////////////////////////

bool Server::SwitchPlayerScene(std::shared_ptr<server::Player>& player, std::shared_ptr<scene::Scene>& new_scene)
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

		log::PrintLine("-> Sending scene object {} via scene switch.", sceneobject->ID);

		auto object = network::constructSceneObjectPacket(sceneobject);
		Send(player->GetPlayerId(), network::PACKETID(network::Packets::SCENEOBJECTNEW), network::Channel::RELIABLE, object);
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

bool Server::AddClientScript(const std::string& name, const std::string& script)
{
	if (m_client_scripts.contains(name))
		return false;

	log::PrintLine(":: Adding client script {}.", name);

	m_client_scripts[name] = script;

	if (IsSinglePlayer())
		return true;

	// Broadcast to players if we are the host.
	if (IsHost() && !IsSinglePlayer())
	{
		packet::ClientScriptAdd packet;
		packet.set_name(name);
		packet.set_script(script);

		Broadcast(PACKETID(Packets::CLIENTSCRIPTADD), network::Channel::RELIABLE, packet);
	}

	return true;
}

bool Server::DeleteClientScript(const std::string& name)
{
	auto iter = m_client_scripts.find(name);
	if (iter == m_client_scripts.end())
		return false;

	m_client_scripts.erase(iter);

	// Broadcast to players if we are the host.
	if (IsHost() && !IsSinglePlayer())
	{
		packet::ClientScriptDelete packet;
		packet.set_name(name);

		Broadcast(PACKETID(Packets::CLIENTSCRIPTDELETE), network::Channel::RELIABLE, packet);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<tdrp::SceneObject> Server::CreateSceneObject(SceneObjectType type, const std::string& object_class, std::shared_ptr<scene::Scene> scene)
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
	std::shared_ptr<tdrp::SceneObject> so = nullptr;
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
	}

	// Claim ownership.
	if (m_player)
	{
		so->SetOwningPlayer(m_player);
		m_player->FollowedSceneObjects.insert(id);
	}

	// Add to the scene.
	scene->AddObject(so);

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

	// Send to players in range.
	auto packet = network::constructSceneObjectPacket(so);
	SendToScene(scene, so->GetPosition(), PACKETID(Packets::SCENEOBJECTNEW), network::Channel::RELIABLE, packet);

	return so;
}

bool Server::DeleteSceneObject(uint32_t id)
{
	auto so = GetSceneObjectById(id);
	return DeleteSceneObject(so);
}

bool Server::DeleteSceneObject(std::shared_ptr<SceneObject> sceneobject)
{
	if (sceneobject == nullptr || !sceneobject->IsGlobal())
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
		auto& graph = scene->GetGraph();
		for (auto it = std::begin(graph); it != std::end(graph);)
		{
			auto sceneobject = it->second;
			if (sceneobject->GetOwningPlayer().lock() == player)
			{
				++count;
				graph.erase(it++);

				packet::SceneObjectDelete packet;
				packet.set_id(sceneobject->ID);
				BroadcastToScene(scene, PACKETID(Packets::SCENEOBJECTDELETE), network::Channel::RELIABLE, packet, { m_player });
			}
			else ++it;
		}
	}

	return count;
}

bool Server::SwitchSceneObjectOwnership(SceneObjectPtr sceneobject, PlayerPtr player)
{
	// Non-replicated objects can't be owned.
	if (sceneobject->NonReplicated)
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

	auto c = iter->second;
	m_object_classes.erase(iter);

	// Broadcast to players if we are the host.
	packet::ClassDelete packet;
	packet.set_name(name);

	Broadcast(PACKETID(Packets::CLASSDELETE), network::Channel::RELIABLE, packet);

	// TODO: Wipe out all script functions bound to this module.

	return c;
}

///////////////////////////////////////////////////////////////////////////////

int Server::SendEvent(std::shared_ptr<scene::Scene> scene, std::shared_ptr<SceneObject> sender, const std::string& name, const std::string& data, Vector2df origin, float radius)
{
	packet::SendEvent packet;
	packet.set_sender(sender ? sender->ID : 0);
	packet.set_name(name);
	packet.set_data(data);
	packet.set_x(origin.x);
	packet.set_y(origin.y);
	packet.set_radius(radius);

	// Run events on all objects in range.
	int count = 0;
	if (scene)
	{
		packet.set_scene(scene->GetName());

		auto targets = scene->FindObjectsBoundInRangeOf(origin, radius);
		for (const auto& target : targets)
			target->OnEvent.RunAll(sender, name, data, origin, radius);

		count = static_cast<int>(targets.size());
	}
	else
	{
		// TODO: All scene objects? Just the server?
		count = 0;
	}

	// Send the packet.
	if (scene)
		SendToScene(scene, origin, network::PACKETID(Packets::SENDEVENT), network::Channel::RELIABLE, packet, { m_player });
	else Broadcast(network::PACKETID(Packets::SENDEVENT), network::Channel::RELIABLE, packet);

	return count;
}

///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<tdrp::SceneObject> Server::GetSceneObjectById(uint32_t id)
{
	for (auto& [key, scene] : m_scenes)
	{
		if (auto so = scene->FindObject(id))
			return so;
	}

	return std::shared_ptr<tdrp::SceneObject>{};
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

std::vector<server::PlayerPtr> Server::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(network::HOSTID, packet_id, channel);
		return {};
	}
	else
		return m_network.SendToScene(scene, location, packet_id, channel, exclude);
}

std::vector<server::PlayerPtr> Server::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(network::HOSTID, packet_id, channel, message);
		return {};
	}
	else
		return m_network.SendToScene(scene, location, packet_id, channel, message, exclude);
}

int Server::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude)
{
	if (IsSinglePlayer() || !IsHost())
	{
		m_network.Send(network::HOSTID, packet_id, channel);
		return 1;
	}
	else
		return m_network.BroadcastToScene(scene, packet_id, channel, exclude);
}

int Server::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude)
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
	// Call the OnPlayerLeave script function.
	auto player = GetPlayerById(id);
	OnPlayerLeave.RunAll(player);

	// Save account.
	player->Account.Save();

	// TODO: Send disconnection packet to peers.
	log::PrintLine("<- Disconnection from player {}.", id);
	m_player_list.erase(id);
}

void Server::network_login(const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	if (packet_id != PACKETID(Packets::LOGIN))
		return;

	log::PrintLine("<- Received login packet from player {}.", id);

	packet::Login packet;
	packet.ParseFromArray(packet_data, static_cast<int>(packet_length));
	
	packet::LoginStatus login_status;
	login_status.set_player_id(id);

	// If this is single player, just accept the login.
	if (IsSinglePlayer())
	{
		// Load account.
		auto player = GetPlayerById(id);
		player->Account.Load("singleplayer");

		// Send response.
		login_status.set_success(true);
		Send(id, PACKETID(Packets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
		log::PrintLine("-> Sending login status - Singleplayer.");
	}
	else
	{
		const auto& method = packet.method();
		const auto& account = packet.account();
		const auto& passwordhash = packet.passwordhash();
		const auto& type = packet.type();
		const auto& version = packet.version();

		// TODO: Properly handle account verification for servers that use it.
		// Load the account.
		auto player = GetPlayerById(id);
		if (method == packet::Login_Method_DEDICATED)
			player->Account.Load(account);
		else
		{
			if (IsHost())
				player->Account.Load("host");
			else player->Account.Load("guest" + std::to_string(id));
		}

		// Login successful.
		if (true)
		{
			login_status.set_success(true);
			Send(id, PACKETID(Packets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
			log::PrintLine("-> Sending login status - success.");
		}
		// Login failure.
		else
		{
			login_status.set_success(false);
			login_status.set_message("Invalid username or password.");
			Send(id, PACKETID(Packets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
			log::PrintLine("-> Sending login status - failure.");

			m_network.DisconnectPeer(id);
			return;
		}
	}

	// Send client control script.
	log::PrintLine("-> Sending client control script to player {}.", id);
	packet::ClientControlScript client_control;
	client_control.set_script(m_client_control_script);
	Send(id, PACKETID(Packets::CLIENTCONTROLSCRIPT), network::Channel::RELIABLE, client_control);

	// Send classes.
	for (const auto& [name, oc] : m_object_classes)
	{
		auto packet = network::constructClassAddPacket(oc);
		Send(id, PACKETID(Packets::CLASSADD), network::Channel::RELIABLE, packet);
	}

	// Send client scripts.
	for (const auto& [name, script] : m_client_scripts)
	{
		log::PrintLine("-> Sending client script {} to player {}.", name, id);
		packet::ClientScriptAdd client_script;
		client_script.set_name(name);
		client_script.set_script(script);
		Send(id, PACKETID(Packets::CLIENTSCRIPTADD), network::Channel::RELIABLE, client_script);
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
	Send(id, PACKETID(Packets::SERVERINFO), network::Channel::RELIABLE, server_info);
}

} // end namespace tdrp::server
