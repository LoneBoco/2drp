#include <chrono>
#include <iostream>

#include "engine/os/Utils.h"

#include "engine/server/Server.h"
#include "engine/server/Account.h"
#include "engine/loader/PackageLoader.h"
#include "engine/loader/LevelLoader.h"
#include "engine/filesystem/File.h"
#include "engine/network/Packet.h"
#include "engine/network/PacketsClient.h"
#include "engine/network/PacketsServer.h"
#include "engine/network/ClientPacketHandler.h"
#include "engine/network/construct/SceneObject.h"

using tdrp::network::PACKETID;
using tdrp::network::ClientPackets;
using tdrp::network::ServerPackets;

namespace tdrp::server
{

std::vector<uint8_t> _serializeMessageToVector(const google::protobuf::Message& message)
{
	size_t message_size = message.ByteSizeLong();

	std::vector<uint8_t> result(message_size);
	message.SerializeToArray(result.data(), message_size);

	return result;
}

/////////////////////////////

Server::Server()
	: m_connecting(false), m_server_type(ServerType::AUTHORITATIVE), m_server_flags(0), m_sceneobject_counter(0), m_server_name("PEER"), m_max_players(8)
{
	// Make up unique id.
	m_unique_id = tdrp::os::CalculateComputerName();

	// Create blank object class.
	m_object_classes.insert(std::make_pair("blank", std::make_shared<ObjectClass>("blank")));

	// Bind the network connect/disconnect callbacks.
	using namespace std::placeholders;
	m_network.SetConnectCallback(std::bind(&Server::network_connect, this, _1));
	m_network.SetDisconnectCallback(std::bind(&Server::network_disconnect, this, _1));
	m_network.SetLoginCallback(std::bind(&Server::network_login, this, _1, _2, _3, _4));

	m_network.BindServer(this);
}

Server::~Server()
{
	network::Network::Shutdown();
}

bool Server::Initialize(const std::string& package_name, const ServerType type, const uint16_t flags)
{
	network::Network::Shutdown();
	if (!network::Network::Startup())
		return false;

	m_server_type = type;
	m_server_flags = flags;

	std::cout << ":: Initializing server." << std::endl;

	// Load the package.
	auto[load_success, package] = loader::PackageLoader::LoadIntoServer(*this, package_name);
	m_package = package;

	// Sanity check for starting scene.
	if (package->GetStartingScene().empty())
		throw std::runtime_error("!! No starting scene for package!");

	// Load everything from the package into the server.
	if (HASFLAG(m_server_flags, ServerFlags::PRELOAD_EVERYTHING))
	{
		std::cout << ":: Loading everything." << std::endl;

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
		std::cout << ":: Loading the starting scene: " << package->GetStartingScene() << "." << std::endl;

		// Load our starting scene.
		auto scene = loader::LevelLoader::CreateScene(*this, package->GetBasePath() / "levels" / package->GetStartingScene());
		if (scene != nullptr)
			m_scenes.insert(std::make_pair(package->GetStartingScene(), scene));
	}

	// Load server script.
	{
		std::cout << ":: Loading server script." << std::endl;
		auto file = FileSystem.GetFile("server.lua");
		if (file)
		{
			auto script = file->ReadAsString();
			Script.RunScript("server", script, *this);
		}
	}

	// Load the client scripts.
	{
		std::cout << ":: Loading client script." << std::endl;
		auto file = FileSystem.GetFile("client.lua");
		if (file)
		{
			m_client_scripts["client"] = file->ReadAsString();
		}
	}

	return true;
}

bool Server::Host(const uint16_t port, const size_t peers)
{
	m_server_type = ServerType::AUTHORITATIVE;
	UNSETFLAG(m_server_flags, ServerFlags::SINGLEPLAYER);

	std::cout << ":: Hosting on port " << port << " for " << peers << " peers." << std::endl;

	m_network.Initialize(peers, port);
	return true;
}

bool Server::Connect(const std::string& hostname, const uint16_t port)
{
	m_server_type = ServerType::SUBORDINATE;
	m_server_flags = 0;

	std::cout << ":: Connecting to " << hostname << " on port " << port << "." << std::endl;

	m_network.Initialize();
	m_connecting = true;
	m_connecting_future = m_network.Connect(hostname, port);
	return true;
}

bool Server::SinglePlayer()
{
	SETFLAG(m_server_flags, ServerFlags::SINGLEPLAYER);

	std::cout << ":: Starting as single player server." << std::endl;

	// Network.Initialize();

	network_connect(0);

	packet::CLogin packet;
	m_network.Send(0, PACKETID(ClientPackets::LOGIN), network::Channel::RELIABLE, packet);
	return true;
}

/////////////////////////////

template <class T>
T getPropsPacket(ObjectAttributes& attributes)
{
	// Create a packet that contains our updated attributes.
	T packet;

	// Loop through all dirty attributes and add them to the packet.
	for (auto& attribute : ObjectAttributes::Dirty(attributes))
	{
		attribute.SetIsDirty(false);

		auto attr = packet.add_attributes();
		attr->set_id(attribute.GetId());

		switch (attribute.GetType())
		{
		case AttributeType::SIGNED:
			attr->set_as_int(attribute.GetSigned());
			break;
		case AttributeType::UNSIGNED:
			attr->set_as_uint(attribute.GetUnsigned());
			break;
		case AttributeType::FLOAT:
			attr->set_as_float(attribute.GetFloat());
			break;
		case AttributeType::DOUBLE:
			attr->set_as_double(attribute.GetDouble());
			break;
		default:
		case AttributeType::STRING:
			attr->set_as_string(attribute.GetString());
			break;
		}
	}

	return packet;
}

void Server::Update(chrono::clock::duration tick)
{
	auto tick_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick).count();

	if (m_connecting)
	{
		if (m_connecting_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			m_connecting = false;

			bool success = m_connecting_future.get();
			if (success)
			{
				std::cout << ":: Connection was successful, sending login packet." << std::endl;

				packet::CLogin packet;
				packet.set_account("test");
				packet.set_passwordhash("testpass");
				packet.set_type(0);
				packet.set_version("first");

				m_network.Send(0, PACKETID(ClientPackets::LOGIN), network::Channel::RELIABLE, packet);
			}
			else
			{
				std::cout << "!! Connection failed." << std::endl;
				m_network.Disconnect();
			}
		}
	}

	// Run server update script.
	OnServerTick.RunAll<Server>(tick_in_ms);

	// Iterate through all the scenes and update all the scene objects in them.
	for (auto& [name, scene] : m_scenes)
	{
		for (auto& [id, object] : scene->m_graph)
		{
			// Run the server tick script on the object.
			object->OnUpdate.RunAll<SceneObject>(tick_in_ms);

			// Check if we have dirty attributes.
			if (object->Attributes.HasDirty())
			{
				// Create a packet that contains our updated attributes.
				auto packet = getPropsPacket<packet::SSceneObjectChange>(object->Attributes);

				//if (!IsSinglePlayer())
				{
					//if (IsHost())
					{
						auto location = object->GetPosition();
						m_network.SendToScene(scene, location.xy(), PACKETID(ServerPackets::SCENEOBJECTCHANGE), network::Channel::RELIABLE, packet);
					}
				}
			}
		}
	}

	// Iterate through all the players and determine if any new scene objects should be sent.
	for (auto& [id, player] : m_player_list)
	{
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

				auto packet = network::constructSceneObjectPacket(item);
				m_network.Send(player->GetPlayerId(), PACKETID(ServerPackets::SCENEOBJECTNEW), network::Channel::RELIABLE, packet);
			});

			player->FollowedSceneObjects.insert(std::begin(new_objects), std::end(new_objects));
		}
	}

	if (!IsSinglePlayer())
		m_network.Update(IsHost());

	FileSystem.Update();
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
	packet::SSwitchScene switchscene;
	switchscene.set_scene(new_scene->GetName());
	m_network.Send(player->GetPlayerId(), network::PACKETID(network::ServerPackets::SWITCHSCENE), network::Channel::RELIABLE, switchscene);

	// Send scene objects to the player.
	// TODO: Use the actual starting position of the player.
	auto sceneobjects = new_scene->FindObjectsInRangeOf({ 0.0f, 0.0f }, static_cast<float>(new_scene->GetTransmissionDistance()));
	for (const auto& sceneobject : sceneobjects)
	{
		player->FollowedSceneObjects.insert(sceneobject->ID);

		auto object = network::constructSceneObjectPacket(sceneobject);
		m_network.Send(player->GetPlayerId(), network::PACKETID(network::ServerPackets::SCENEOBJECTNEW), network::Channel::RELIABLE, object);
	}

	return true;
}

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
		m_network.Send(player->GetPlayerId(), PACKETID(network::ServerPackets::SCENEOBJECTNEW), network::Channel::RELIABLE, packet_newso);
	}

	packet::SSceneObjectControl packet;
	packet.set_old_id(old_id);
	packet.set_new_id(new_scene_object->ID);
	m_network.Send(player->GetPlayerId(), network::PACKETID(network::ServerPackets::SCENEOBJECTCONTROL), network::Channel::RELIABLE, packet);

	return true;
}

///////////////////////////////////////////////////////////////////////////////

void Server::AddClientScript(const std::string& name, const std::string& script)
{
	m_client_scripts[name] = script;

	// Broadcast to players if we are the host.
	if (IsHost() && !IsSinglePlayer())
	{
		packet::SClientScript packet;
		packet.set_name(name);
		packet.set_script(script);

		m_network.Broadcast(PACKETID(ServerPackets::CLIENTSCRIPT), network::Channel::RELIABLE, packet);
	}
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
		packet::SClientScriptDelete packet;
		packet.set_name(name);

		m_network.Broadcast(PACKETID(ServerPackets::CLIENTSCRIPTDELETE), network::Channel::RELIABLE, packet);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<tdrp::SceneObject> Server::CreateSceneObject(SceneObjectType type, const std::string& object_class, std::shared_ptr<scene::Scene> scene)
{
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
	}

	// Add to the scene.
	scene->AddObject(so);

	// Handle the script.
	if (IsHost() && so && oc)
	{
		Script.RunScript("sceneobject_" + std::to_string(id) + "_class_" + oc->GetName(), oc->ScriptServer, so);
	}

	// The scene object will be sent to players when the server determines it is in range.

	return so;
}

bool Server::DeleteSceneObject(uint32_t id)
{
	for (auto& [key, scene] : m_scenes)
	{
		if (auto so = scene->FindObject(id))
		{
			if (!so->IsGlobal())
				return false;

			m_scenes.erase(key);
			return true;
		}
	}

	return false;
}

bool Server::DeleteSceneObject(std::shared_ptr<SceneObject> sceneobject)
{
	for (auto& [key, scene] : m_scenes)
	{
		if (auto so = scene->FindObject(sceneobject->ID))
		{
			if (!so->IsGlobal())
				return false;

			m_scenes.erase(key);
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ObjectClass> Server::DeleteObjectClass(const std::string& name)
{
	auto iter = m_object_classes.find(name);
	if (iter == m_object_classes.end())
		return nullptr;

	auto c = iter->second;
	m_object_classes.erase(iter);

	// Broadcast to players if we are the host.
	if (IsHost())
	{
		packet::SClassDelete packet;
		packet.set_name(name);

		m_network.Broadcast(PACKETID(ServerPackets::CLASSDELETE), network::Channel::RELIABLE, packet);
	}

	// TODO: Wipe out all script functions bound to this module.

	return c;
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
	if (IsSinglePlayer() || IsHost())
		tdrp::network::handlers::network_receive(this, 0, packet_id, nullptr, 0);
	else m_network.Send(peer_id, packet_id, channel);
}

void Server::Send(const uint16_t peer_id, const uint16_t packet_id, const network::Channel channel, google::protobuf::Message& message)
{
	if (IsSinglePlayer() || IsHost())
	{
		auto data = _serializeMessageToVector(message);
		tdrp::network::handlers::network_receive(this, 0, packet_id, data.data(), data.size());
	}
	else m_network.Send(peer_id, packet_id, channel, message);
}

void Server::Broadcast(const uint16_t packet_id, const network::Channel channel)
{
	if (IsSinglePlayer() || IsHost())
		tdrp::network::handlers::network_receive(this, 0, packet_id, nullptr, 0);
	else m_network.Broadcast(packet_id, channel);
}

void Server::Broadcast(const uint16_t packet_id, const network::Channel channel, google::protobuf::Message& message)
{
	if (IsSinglePlayer() || IsHost())
	{
		auto data = _serializeMessageToVector(message);
		tdrp::network::handlers::network_receive(this, 0, packet_id, data.data(), data.size());
	}
	else m_network.Broadcast(packet_id, channel, message);
}

std::vector<server::PlayerPtr> Server::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, uint16_t packet_id, const network::Channel channel)
{
	if (IsSinglePlayer() || IsHost())
	{
		tdrp::network::handlers::network_receive(this, 0, packet_id, nullptr, 0);
		return {};
	}
	else return m_network.SendToScene(scene, location, packet_id, channel);
}

std::vector<server::PlayerPtr> Server::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const network::Channel channel, google::protobuf::Message& message)
{
	if (IsSinglePlayer() || IsHost())
	{
		auto data = _serializeMessageToVector(message);
		tdrp::network::handlers::network_receive(this, 0, packet_id, data.data(), data.size());
		return {};
	}
	else return m_network.SendToScene(scene, location, packet_id, channel, message);
}

int Server::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const network::Channel channel)
{
	if (IsSinglePlayer() || IsHost())
	{
		tdrp::network::handlers::network_receive(this, 0, packet_id, nullptr, 0);
		return 1;
	}
	else return m_network.BroadcastToScene(scene, packet_id, channel);
}

int Server::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const network::Channel channel, google::protobuf::Message& message)
{
	if (IsSinglePlayer() || IsHost())
	{
		auto data = _serializeMessageToVector(message);
		tdrp::network::handlers::network_receive(this, 0, packet_id, data.data(), data.size());
		return 1;
	}
	else return m_network.BroadcastToScene(scene, packet_id, channel, message);
}

/////////////////////////////

void Server::network_connect(const uint16_t id)
{
	std::cout << "<- Connection from " << id << "." << std::endl;
	auto player = std::make_shared<Player>(id);
	player->BindServer(this);
	m_player_list[id] = player;
}

void Server::network_disconnect(const uint16_t id)
{
	// Call the OnPlayerLeave script function.
	auto player = GetPlayerById(id);
	OnPlayerLeave.RunAll<Server>(player);

	// TODO: Send disconnection packet to peers.
	std::cout << "<- Disconnection from " << id << "." << std::endl;
	m_player_list.erase(id);
}

void Server::network_login(const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	if (packet_id != PACKETID(ClientPackets::LOGIN))
		return;

	std::cout << "<- Received login packet!" << std::endl;

	packet::CLogin packet;
	packet.ParseFromArray(packet_data, packet_length);

	// Bind the account.
	auto account = std::make_unique<server::Account>();
	m_network.BindAccountToPeer(id, std::move(account));

	packet::SLoginStatus login_status;

	// If this is single player, just accept the login.
	if (IsSinglePlayer())
	{
		login_status.set_success(true);
		m_network.Send(id, PACKETID(ServerPackets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
		std::cout << "-> Sending login status - Singleplayer." << std::endl;
	}
	else
	{

		// TODO: Properly handle account verification for servers that use it.

		// Login successful.
		if (true)
		{
			login_status.set_success(true);
			m_network.Send(id, PACKETID(ServerPackets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
			std::cout << "-> Sending login status - success." << std::endl;
		}
		// Login failure.
		else
		{
			login_status.set_success(false);
			login_status.set_message("Invalid username or password.");
			m_network.Send(id, PACKETID(ServerPackets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
			std::cout << "-> Sending login status - failure." << std::endl;

			m_network.DisconnectPeer(id);
			return;
		}
	}

	// Send client control script.
	for (const auto& [name, script] : m_client_scripts)
	{
		packet::SClientScript client_script;
		client_script.set_name(name);
		client_script.set_script(script);
		m_network.Send(id, PACKETID(ServerPackets::CLIENTSCRIPT), network::Channel::RELIABLE, client_script);
	}

	// Send server info.
	// Keep this last.  The client will start the load after this packet.
	packet::SServerInfo server_info;
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
	m_network.Send(id, PACKETID(ServerPackets::SERVERINFO), network::Channel::RELIABLE, server_info);
}

} // end namespace tdrp::server
