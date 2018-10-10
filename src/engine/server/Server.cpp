#include <chrono>
#include <iostream>

#include "engine/server/Server.h"
#include "engine/server/Account.h"
#include "engine/loader/PackageLoader.h"
#include "engine/loader/LevelLoader.h"
#include "engine/filesystem/File.h"
#include "engine/network/PacketID.h"
#include "engine/network/PacketsClient.h"
#include "engine/network/PacketsServer.h"

namespace tdrp::server
{

Server::Server()
	: m_connecting(false), m_server_type(ServerType::AUTHORITATIVE), m_server_flags(0), m_sceneobject_counter(0)
{
	// Create blank object class.
	m_object_classes.insert(std::make_pair("blank", std::make_shared<ObjectClass>("blank")));

	// Bind the network connect/disconnect callbacks.
	using namespace std::placeholders;
	Network.SetConnectCallback(std::bind(&Server::network_connect, this, _1));
	Network.SetDisconnectCallback(std::bind(&Server::network_disconnect, this, _1));
	Network.SetLoginCallback(std::bind(&Server::network_login, this, _1, _2, _3, _4));

	Network.BindServer(this);
}

Server::~Server()
{
	if (!IsSinglePlayer())
		Network.Shutdown();
}

bool Server::Initialize(const std::string& package_name, const ServerType type, const uint16_t flags)
{
	Network.Shutdown();

	m_server_type = type;
	m_server_flags = flags;

	std::cout << ":: Initializing server." << std::endl;

	auto[load_success, package] = loader::PackageLoader::LoadIntoServer(*this, package_name);

	// Load everything.
	if (m_server_flags & static_cast<uint16_t>(ServerFlags::PRELOAD_EVERYTHING))
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
	else
	{
		std::cout << ":: Loading the starting scene: " << package->GetStartingScene() << "." << std::endl;

		// Load our starting scene.
		auto scene = loader::LevelLoader::CreateScene(*this, package->GetBasePath() / "levels" / package->GetStartingScene());
		if (scene != nullptr)
			m_scenes.insert(std::make_pair(package->GetStartingScene(), scene));
	}

	return true;
}

bool Server::Host(const uint16_t port, const size_t peers)
{
	if (IsSinglePlayer())
		return false;

	if (!IsHost())
		return false;

	if (!network::Network::Startup())
		return false;

	std::cout << ":: Hosting on port " << port << " for " << peers << " peers." << std::endl;

	Network.Initialize(peers, port);
	return true;
}

bool Server::Connect(const std::string& hostname, const uint16_t port)
{
	m_server_type = ServerType::SUBORDINATE;
	m_server_flags = 0;

	if (!network::Network::Startup())
		return false;

	std::cout << ":: Connecting to " << hostname << " on port " << port << "." << std::endl;

	Network.Initialize();
	m_connecting = true;
	m_connecting_future = Network.Connect(hostname, port);
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

void Server::Update()
{
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

				Network.Send(0, static_cast<uint16_t>(ClientPackets::LOGIN), network::Channel::RELIABLE, packet);
			}
			else
			{
				std::cout << "!! Connection failed." << std::endl;
				Network.Disconnect();
			}
		}
	}

	// Find any dirty attributes and update them.
	for (auto&[name, scene] : m_scenes)
	{
		// Loop through all scene objects.
		for (auto&[id, object] : scene->m_graph)
		{
			// Check if we have dirty attributes.
			if (object->Attributes.HasDirty())
			{
				// Create a packet that contains our updated attributes.
				auto packet = getPropsPacket<packet::SSceneObjectChange>(object->Attributes);

				if (!IsSinglePlayer())
				{
					if (IsHost())
					{
						auto location = object->GetPosition();
						Network.SendToScene(*scene, location.xy(), static_cast<uint16_t>(ServerPackets::SCENEOBJECTCHANGE), network::Channel::RELIABLE, packet);
					}
				}
			}
		}
	}

	if (!IsSinglePlayer())
		Network.Update(IsHost());
}

///////////////////////////////////////////////////////////////////////////////

void Server::AddClientScript(const std::string& name, const std::string& script)
{
	m_client_scripts[name] = script;

	// Broadcast to players if we are the host.
	if (IsHost())
	{
		packet::SClientScript packet;
		packet.set_name(name);
		packet.set_script(script);

		Network.Broadcast(static_cast<uint16_t>(ServerPackets::CLIENTSCRIPT), network::Channel::RELIABLE, packet);
	}
}

///////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ObjectClass> Server::GetObjectClass(const std::string& name)
{
	auto iter = m_object_classes.find(name);
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

		Network.Broadcast(static_cast<uint16_t>(ServerPackets::CLASSDELETE), network::Channel::RELIABLE, packet);
	}

	return c;
}

bool Server::DeleteClientScript(const std::string& name)
{
	auto iter = m_client_scripts.find(name);
	if (iter == m_client_scripts.end())
		return false;

	m_client_scripts.erase(iter);

	// Broadcast to players if we are the host.
	if (IsHost())
	{
		packet::SClientScriptDelete packet;
		packet.set_name(name);

		Network.Broadcast(static_cast<uint16_t>(ServerPackets::CLIENTSCRIPTDELETE), network::Channel::RELIABLE, packet);
	}

	return true;
}

/////////////////////////////

void Server::network_connect(const uint16_t id)
{
	// This shouldn't happen in a single player server.
	// TODO: Disconnect?
	if (IsSinglePlayer())
		return;

	std::cout << "<- Connection from " << id << "." << std::endl;
	m_player_list[id] = std::make_shared<Player>(id);
}

void Server::network_disconnect(const uint16_t id)
{
	// TODO: Send disconnection packet to peers.
	std::cout << "<- Disconnection from " << id << "." << std::endl;
	m_player_list.erase(id);
}

void Server::network_login(const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	if (packet_id != static_cast<uint16_t>(ClientPackets::LOGIN))
		return;

	std::cout << "<- Received login packet!" << std::endl;

	packet::CLogin packet;
	packet.ParseFromArray(packet_data, packet_length);

	auto account = std::make_unique<server::Account>();

	// TODO: Verify account is valid.

	// TODO: Notify client that they are logged in or rejected.
	packet::SLoginStatus login_status;
	login_status.set_success(true);
	//login_status.set_message("Invalid username or password.");
	Network.Send(id, static_cast<uint16_t>(ServerPackets::LOGINSTATUS), network::Channel::RELIABLE, login_status);
	std::cout << "-> Sending login status." << std::endl;

	// If accepted, bind the account.
	Network.BindAccountToPeer(id, std::move(account));
}

} // end namespace tdrp::server
