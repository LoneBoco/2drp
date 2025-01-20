#pragma once

#include <future>

#include "engine/common.h"

#include "engine/filesystem/FileSystem.h"
#include "engine/network/Network.h"
#include "engine/network/DownloadManager.h"
#include "engine/item/Item.h"
#include "engine/resources/IdGenerator.h"
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

enum class ServerFlags : uint8_t
{
	//! Loads all resources at the start instead of on-demand.
	PRELOAD_EVERYTHING = 0b0000'0001,

	//! Single player mode.
	SINGLEPLAYER = 0b0000'0010,
};

typedef std::function<void(SceneObjectPtr so)> sceneobject_add_cb;

class Server
{
	friend class tdrp::Loader;
	friend class network::Network;

	SCRIPT_SETUP;
	SCRIPT_FUNCTION(OnStarted);
	SCRIPT_FUNCTION(OnPlayerJoin);
	SCRIPT_FUNCTION(OnPlayerLeave);
	SCRIPT_FUNCTION(OnServerTick);
	SCRIPT_FUNCTION(OnServerEvent);
	SCRIPT_FUNCTION(OnEvent);

public:
	Server();
	~Server();

	Server(const Server& other) = delete;
	Server(Server&& other) = delete;
	Server& operator=(const Server& other) = delete;
	Server& operator=(Server&& other) = delete;

public:
	bool Initialize(const ServerType type, const uint16_t flags);
	bool LoadPackage(const std::string& package_name);
	bool HostDedicated(const uint16_t port, const size_t peers = 32);
	bool Host(const uint16_t port, const size_t peers = 32);
	bool Connect(const std::string& hostname, const uint16_t port);
	bool SinglePlayer();

public:
	void Shutdown();
	bool IsShuttingDown() const noexcept;

public:
	void PreUpdate();
	void Update(const std::chrono::milliseconds& tick);

public:
	void ProcessPlayerJoin(const uint16_t player_id);
	void ProcessPlayerLogin(const uint16_t player_id, const std::string& account);

public:
	void SetUniqueId(const std::string& id);
	void SetServerName(const std::string& name);
	void SetMaxPlayers(const uint32_t max_players);
	void SetPlayerNumber(const uint16_t player_number);
	const std::string& GetUniqueId() const;
	const std::string& GetServerName() const;
	const uint16_t GetMaxPlayers() const;
	const PlayerPtr GetPlayer() const;
	std::shared_ptr<package::Package> GetPackage() const;

public:
	std::shared_ptr<ObjectClass> GetObjectClass(const std::string& name);
	std::shared_ptr<scene::Tileset> GetTileset(const std::string& name) const;
	scene::ScenePtr GetScene(const std::string& name) const;
	scene::ScenePtr GetOrCreateScene(const std::string& name) noexcept;

public:
	void LoadServerScript(const std::string& name, const std::string& script);
	void LoadClientScript(const std::string& name, const std::string& script, bool required);
	void EraseClientScript(const std::string& name);
	void AddPlayerClientScript(const std::string& name, PlayerPtr player);
	void RemovePlayerClientScript(const std::string& name, PlayerPtr player);
	std::unordered_map<std::string, std::pair<bool, std::string>>& GetClientScriptMap();

public:
	bool AddItemDefinition(item::ItemDefinitionUPtr&& item);
	void SendItemDefinition(server::PlayerPtr player, ItemID baseId);
	item::ItemDefinition* GetItemDefinition(ItemID baseId);
	item::ItemDefinition* GetItemDefinition(const std::string& name);
	item::ItemInstancePtr GiveItemToPlayer(server::PlayerPtr player, ItemID baseId, item::ItemType type, size_t count = 1);
	item::ItemInstancePtr GiveVariantItemToPlayer(server::PlayerPtr player, ItemID baseId, const ObjectAttributes& attributes);
	item::ItemInstancePtr RemoveItemFromPlayer(server::PlayerPtr player, ItemID itemId, size_t count = 1);
	item::ItemInstancePtr SetItemCount(server::PlayerPtr player, ItemID itemId, size_t count);

public:
	void SetAccountFlag(server::PlayerPtr player, const std::string& flag, const auto& value);
	AttributePtr GetAccountFlag(const server::PlayerPtr player, const std::string& flag) const;

public:
	SceneObjectPtr CreateSceneObject(SceneObjectType type, const std::string& object_class, SceneObjectID = 0);
	SceneObjectPtr AddSceneObject(SceneObjectPtr sceneobject);
	bool DeleteSceneObject(SceneObjectID id);
	bool DeleteSceneObject(SceneObjectPtr sceneobject);
	size_t DeletePlayerOwnedSceneObjects(PlayerPtr player);
	bool SwitchSceneObjectScene(SceneObjectPtr sceneobject, scene::ScenePtr scene);
	bool SwitchSceneObjectOwnership(SceneObjectPtr sceneobject, PlayerPtr player);
	void RequestSceneObjectChunkData(SceneObjectPtr sceneobject, uint32_t chunk_idx);

public:
	// AddObjectClass
	std::shared_ptr<ObjectClass> DeleteObjectClass(const std::string& name);

public:
	bool AddTileset(const std::string& name, scene::TilesetPtr tileset);

public:
	bool SwitchPlayerScene(PlayerPtr& player, scene::ScenePtr& new_scene);

public:
	int SendEvent(scene::ScenePtr scene, PlayerPtr player, const std::string& name, const std::string& data);
	int SendEvent(scene::ScenePtr scene, PlayerPtr player, const std::string& name, const std::string& data, Vector2df origin, float radius);
	int ProcessSendEvent(scene::ScenePtr scene, PlayerPtr player, const std::string& name, const std::string& data) const;
	int ProcessSendEvent(scene::ScenePtr scene, PlayerPtr player, const std::string& name, const std::string& data, Vector2df origin, float radius);

public:
	const SceneObjectID GetNextSceneObjectID();
	const bool IsSinglePlayer() const;
	const bool IsHost() const;
	const bool IsGuest() const;
	const ServerType GetServerType() const;
	PlayerPtr GetPlayerById(uint16_t id);
	SceneObjectPtr GetSceneObjectById(SceneObjectID id);

public:
	void Send(const uint16_t peer_id, const uint16_t packet_id, const network::Channel channel);
	void Send(const uint16_t peer_id, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message);
	void Broadcast(const uint16_t packet_id, const network::Channel channel);
	void Broadcast(const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message);
	std::vector<server::PlayerPtr> SendToScene(const scene::ScenePtr scene, const Vector2df location, uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude = {});
	std::vector<server::PlayerPtr> SendToScene(const scene::ScenePtr scene, const Vector2df location, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude = {});
	int BroadcastToScene(const scene::ScenePtr scene, const uint16_t packet_id, const network::Channel channel, const std::set<PlayerPtr>& exclude = {});
	int BroadcastToScene(const scene::ScenePtr scene, const uint16_t packet_id, const network::Channel channel, const google::protobuf::Message& message, const std::set<PlayerPtr>& exclude = {});

public:
	//void SetClientNetworkReceiveCallback(network::enet_receive_cb callback);
	network::Network& GetNetwork();

public:
	tdrp::fs::FileSystem FileSystem;
	filesystem::path DefaultDownloadPath;
	network::DownloadManager DownloadManager;

public:
	script::ScriptPtr Script;

public:
	sceneobject_add_cb OnSceneObjectAdd;
	sceneobject_add_cb OnSceneObjectRemove;

protected:
	void network_connect(const uint16_t id);
	void network_disconnect(const uint16_t id);
	void network_login(const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length);

protected:
	network::Network m_network;

	bool m_shutting_down = false;
	bool m_connecting;
	std::future<bool> m_connecting_future;

	ServerType m_server_type;
	uint16_t m_server_flags;

	IdGenerator<SceneObjectID> m_sceneobject_ids;

	std::shared_ptr<package::Package> m_package;
	std::unordered_map<std::string, std::shared_ptr<ObjectClass>> m_object_classes;
	std::unordered_map<std::string, std::shared_ptr<scene::Tileset>> m_tilesets;
	std::unordered_map<std::string, scene::ScenePtr> m_scenes;
	std::unordered_map<std::string, std::string> m_server_scripts;
	std::unordered_map<std::string, std::pair<bool, std::string>> m_client_scripts;
	std::unordered_map<ItemID, item::ItemDefinitionUPtr> m_item_definitions;

	std::unordered_map<PlayerID, PlayerPtr> m_player_list;

	std::string m_unique_id;
	std::string m_server_name;
	uint16_t m_max_players;

	PlayerPtr m_player = std::make_shared<Player>(0);
};

inline bool Server::IsShuttingDown() const noexcept
{
	return m_shutting_down;
}

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

inline std::shared_ptr<package::Package> Server::GetPackage() const
{
	return m_package;
}

inline const SceneObjectID Server::GetNextSceneObjectID()
{
	return m_sceneobject_ids.GetNextId();
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

inline PlayerPtr Server::GetPlayerById(uint16_t id)
{
	auto iter = m_player_list.find(id);
	if (iter == std::end(m_player_list))
		return nullptr;
	return iter->second;
}

inline network::Network& Server::GetNetwork()
{
	return m_network;
}

inline std::unordered_map<std::string, std::pair<bool, std::string>>& Server::GetClientScriptMap()
{
	return m_client_scripts;
}

} // end namespace tdrp::server
