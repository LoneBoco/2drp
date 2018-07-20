#pragma once

#include <future>

#include "engine/common.h"

#include "engine/filesystem/FileSystem.h"
#include "engine/network/Network.h"
#include "engine/scene/ObjectClass.h"
#include "engine/scene/Scene.h"
#include "engine/scene/Tileset.h"

namespace tdrp::server
{

enum class ServerType
{
	AUTHORITATIVE,
	SUBORDINATE,
};

enum class ServerFlags : uint16_t
{
	//! Loads all resources at the start instead of on-demand.
	PRELOAD_EVERYTHING = 0b00000001,

	//! Only single player mode, don't start up the network.
	SINGLEPLAYER_ONLY = 0b00000010,
};

class Server
{
	friend class loader::PackageLoader;

public:
	Server();
	~Server();

	Server(const Server& other) = delete;
	Server(Server&& other) = delete;
	Server& operator=(const Server& other) = delete;
	Server& operator=(Server&& other) = delete;

public:
	bool Initialize(const std::string& package_name, const ServerType type, const uint16_t flags);
	bool Host(const uint16_t port, const size_t peers = 32);
	bool Connect(const std::string& hostname, const uint16_t port);

public:
	void Update();

public:
	std::shared_ptr<ObjectClass> GetObjectClass(const std::string& name);
	std::shared_ptr<scene::Tileset> GetTileset(const std::string& name);
	std::shared_ptr<scene::Scene> GetScene(const std::string& name);

	const uint32_t GetNextSceneObjectID();
	const bool IsSinglePlayer() const;

	network::Network Network;

protected:
	void network_connect(const uint16_t id);
	void network_disconnect(const uint16_t id);
	void network_login(const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length);

protected:
	bool m_connecting;
	std::future<bool> m_connecting_future;

	ServerType m_server_type;
	uint16_t m_server_flags;

	uint32_t m_sceneobject_counter;
	fs::FileSystem m_filesystem;
	std::map<std::string, std::shared_ptr<ObjectClass>> m_object_classes;
	std::map<std::string, std::shared_ptr<scene::Tileset>> m_tilesets;
	std::map<std::string, std::shared_ptr<scene::Scene>> m_scenes;
	std::string m_client_script;
};

inline const uint32_t Server::GetNextSceneObjectID()
{
	return ++m_sceneobject_counter;
}

inline const bool Server::IsSinglePlayer() const
{
	return m_server_flags & static_cast<uint16_t>(ServerFlags::SINGLEPLAYER_ONLY);
}

} // end namespace tdrp::server
