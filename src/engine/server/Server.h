#pragma once

#include <future>

#include "engine/common.h"

#include "engine/filesystem/FileSystem.h"
#include "engine/network/Network.h"
#include "engine/network/DownloadManager.h"
#include "engine/scene/ObjectClass.h"
#include "engine/scene/Scene.h"
#include "engine/scene/Tileset.h"
#include "engine/server/Player.h"
#include "engine/script/Function.h"
#include "engine/script/Script.h"

namespace tdrp::server
{

enum class ServerType
{
	HOST,
	GUEST,
	DEDICATED
};

enum class ServerFlags : uint16_t
{
	//! Loads all resources at the start instead of on-demand.
	PRELOAD_EVERYTHING = 0b0000'0001,

	//! Single player mode.
	SINGLEPLAYER = 0b0000'0010,
};

class Server
{
	friend class loader::PackageLoader;
	friend class network::Network;

	SCRIPT_SETUP;
	SCRIPT_FUNCTION(OnStarted);
	SCRIPT_FUNCTION(OnPlayerJoin);
	SCRIPT_FUNCTION(OnPlayerLeave);
	SCRIPT_FUNCTION(OnServerTick);

public:
	Server();
	~Server();

	Server(const Server& other) = delete;
	Server(Server&& other) = delete;
	Server& operator=(const Server& other) = delete;
	Server& operator=(Server&& other) = delete;

public:
	bool Initialize(const std::string& package_name, const ServerType type, const uint16_t flags);
	bool HostDedicated(const uint16_t port, const size_t peers = 32);
	bool Host(const uint16_t port, const size_t peers = 32);
	bool Connect(const std::string& hostname, const uint16_t port);
	bool SinglePlayer();

public:
	void PreUpdate();
	void Update(chrono::clock::duration tick);

public:
	void SetUniqueId(const std::string& id);
	void SetServerName(const std::string& name);
	void SetMaxPlayers(const uint32_t max_players);
	void SetPlayerNumber(const uint16_t player_number);
	const std::string& GetUniqueId() const;
	const std::string& GetServerName() const;
	const uint16_t GetMaxPlayers() const;
	const PlayerPtr GetPlayer() const;
	std::shared_ptr<package::Package> GetPackage();

public:
	std::shared_ptr<ObjectClass> GetObjectClass(const std::string& name);
	std::shared_ptr<scene::Tileset> GetTileset(const std::string& name);
	std::shared_ptr<scene::Scene> GetScene(const std::string& name);

public:
	void AddClientScript(const std::string& name, const std::string& script);
	bool DeleteClientScript(const std::string& name);
	std::map<std::string, std::string>& GetClientScriptMap();

public:
	std::shared_ptr<tdrp::SceneObject> CreateSceneObject(SceneObjectType type, const std::string& object_class, std::shared_ptr<scene::Scene> scene);
	bool DeleteSceneObject(uint32_t id);
	bool DeleteSceneObject(std::shared_ptr<SceneObject> sceneobject);
	size_t DeletePlayerOwnedSceneObjects(PlayerPtr player);
	bool SwitchSceneObjectOwnership(SceneObjectPtr sceneobject, PlayerPtr player);

public:
	// AddObjectClass
	std::shared_ptr<ObjectClass> DeleteObjectClass(const std::string& name);

public:
	bool SwitchPlayerScene(std::shared_ptr<server::Player>& player, std::shared_ptr<scene::Scene>& new_scene);
	//bool SwitchPlayerControlledSceneObject(std::shared_ptr<server::Player>& player, std::shared_ptr<SceneObject>& new_scene_object);

public:
	int SendEvent(std::shared_ptr<scene::Scene> scene, std::shared_ptr<SceneObject> sender, const std::string& name, const std::string& data, Vector2df origin, float radius);

public:
	const uint32_t GetNextSceneObjectID();
	const bool IsSinglePlayer() const;
	const bool IsHost() const;
	const bool IsGuest() const;
	const ServerType GetServerType() const;
	std::shared_ptr<server::Player> GetPlayerById(uint16_t id);
	std::shared_ptr<tdrp::SceneObject> GetSceneObjectById(uint32_t id);

public:
	void Send(const uint16_t peer_id, const uint16_t packet_id, const network::Channel channel);
	void Send(const uint16_t peer_id, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message);
	void Broadcast(const uint16_t packet_id, const network::Channel channel);
	void Broadcast(const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message);
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude = {});
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude = {});
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude = {});
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude = {});

public:
	//void SetClientNetworkReceiveCallback(network::enet_receive_cb callback);
	network::Network& GetNetwork();

public:
	tdrp::fs::FileSystem FileSystem;
	filesystem::path DefaultDownloadPath;
	network::DownloadManager DownloadManager;

public:
	script::ScriptPtr Script;

protected:
	void network_connect(const uint16_t id);
	void network_disconnect(const uint16_t id);
	void network_login(const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length);

protected:
	network::Network m_network;

	bool m_connecting;
	std::future<bool> m_connecting_future;

	ServerType m_server_type;
	uint16_t m_server_flags;

	uint32_t m_sceneobject_counter;

	std::shared_ptr<package::Package> m_package;
	std::map<std::string, std::shared_ptr<ObjectClass>> m_object_classes;
	std::map<std::string, std::shared_ptr<scene::Tileset>> m_tilesets;
	std::map<std::string, std::shared_ptr<scene::Scene>> m_scenes;
	std::map<std::string, std::string> m_client_scripts;

	std::map<uint16_t, PlayerPtr> m_player_list;

	std::string m_unique_id;
	std::string m_server_name;
	uint16_t m_max_players;
	PlayerPtr m_player;
};

inline void Server::SetUniqueId(const std::string& id)
{
	m_unique_id = id;
}

inline void Server::SetServerName(const std::string& name)
{
	m_server_name = name;
}

inline void Server::SetMaxPlayers(const uint32_t max_players)
{
	m_max_players = max_players;
}

inline const std::string& Server::GetUniqueId() const
{
	return m_unique_id;
}

inline const std::string& Server::GetServerName() const
{
	return m_server_name;
}

inline const uint16_t Server::GetMaxPlayers() const
{
	return m_max_players;
}

inline const PlayerPtr Server::GetPlayer() const
{
	return m_player;
}

inline std::shared_ptr<package::Package> Server::GetPackage()
{
	return m_package;
}

inline const uint32_t Server::GetNextSceneObjectID()
{
	return ++m_sceneobject_counter;
}

inline const bool Server::IsSinglePlayer() const
{
	return HASFLAG(m_server_flags, ServerFlags::SINGLEPLAYER);
}

inline const bool Server::IsHost() const
{
	return m_server_type == ServerType::HOST || m_server_type == ServerType::DEDICATED;
}

inline const bool Server::IsGuest() const
{
	return m_server_type == ServerType::GUEST;
}

inline const ServerType Server::GetServerType() const
{
	return m_server_type;
}

inline std::shared_ptr<server::Player> Server::GetPlayerById(uint16_t id)
{
	return m_player_list[id];
}

inline network::Network& Server::GetNetwork()
{
	return m_network;
}

inline std::map<std::string, std::string>& Server::GetClientScriptMap()
{
	return m_client_scripts;
}

} // end namespace tdrp::server
