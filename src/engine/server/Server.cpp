#include <chrono>
#include <iostream>

#include "engine/server/Server.h"
#include "engine/server/Account.h"
#include "engine/loader/PackageLoader.h"
#include "engine/loader/LevelLoader.h"
#include "engine/filesystem/File.h"
#include "engine/network/PacketID.h"
#include "engine/network/PacketsClient.h"

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

	std::cout << "Initializing server" << std::endl;

	auto[load_success, package] = loader::PackageLoader::LoadIntoServer(*this, package_name);

	// Load everything.
	if (m_server_flags & static_cast<uint16_t>(ServerFlags::PRELOAD_EVERYTHING))
	{
		std::cout << "Loading everything" << std::endl;

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
		std::cout << "Loading the starting scene" << std::endl;

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

	if (m_server_type != ServerType::AUTHORITATIVE)
		return false;

	if (!network::Network::Startup())
		return false;

	std::cout << "Hosting on port " << port << " for " << peers << " peers" << std::endl;

	Network.Initialize(peers, port);
	return true;
}

bool Server::Connect(const std::string& hostname, const uint16_t port)
{
	m_server_type = ServerType::SUBORDINATE;
	m_server_flags = 0;

	if (!network::Network::Startup())
		return false;

	std::cout << "Connecting to " << hostname << " on port " << port << std::endl;

	Network.Initialize();
	m_connecting = true;
	m_connecting_future = Network.Connect(hostname, port);
	return true;
}

/////////////////////////////

void Server::Update()
{
	if (m_connecting)
	{
		if (m_connecting_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			m_connecting = false;

			std::cout << "Connection finished" << std::endl;

			bool success = m_connecting_future.get();
			if (success)
			{
				std::cout << "Connection was successful, sending packet" << std::endl;

				packet::CLogin packet;
				packet.set_account("test");
				packet.set_passwordhash("testpass");
				packet.set_type(0);
				packet.set_version("first");

				Network.Send(0, static_cast<uint16_t>(ClientPackets::LOGIN), network::Channel::RELIABLE, packet);
			}
		}
	}

	if (!IsSinglePlayer())
		Network.Update(m_server_type == ServerType::AUTHORITATIVE);
}

std::shared_ptr<ObjectClass> Server::GetObjectClass(const std::string& name)
{
	auto iter = m_object_classes.find(name);
	if (iter == m_object_classes.end())
		return nullptr;
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

/////////////////////////////

void Server::network_connect(const uint16_t id)
{
	// This shouldn't happen in a single player server.
	// TODO: Disconnect?
	if (IsSinglePlayer())
		return;

	std::cout << "connection from " << id << std::endl;
}

void Server::network_disconnect(const uint16_t id)
{
	// TODO: Send disconnection packet to peers.
	std::cout << "disconnection from " << id << std::endl;
}

void Server::network_login(const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	if (packet_id != static_cast<uint16_t>(ClientPackets::LOGIN))
		return;

	std::cout << "login packet!" << std::endl;

	packet::CLogin packet;
	packet.ParseFromArray(packet_data, packet_length);

	auto account = std::make_unique<server::Account>();

	// TODO: Verify account is valid.

	// TODO: Notify client that they are logged in or rejected.

	// If accepted, bind the account.
	Network.BindAccountToPeer(id, std::move(account));
}

} // end namespace tdrp::server
