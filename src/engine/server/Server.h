#pragma once

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
	~Server() = default;

	Server(const Server& other) = delete;
	Server(Server&& other) = delete;
	Server& operator=(const Server& other) = delete;
	Server& operator=(Server&& other) = delete;

	bool Initialize(const std::string& package_name, const ServerType type, const uint16_t flags);

	void Update();

	std::shared_ptr<ObjectClass> GetObjectClass(const std::string& name);
	std::shared_ptr<scene::Tileset> GetTileset(const std::string& name);
	std::shared_ptr<scene::Scene> GetScene(const std::string& name);

	const uint32_t GetNextSceneObjectID()
	{
		return ++m_sceneobject_counter;
	}

	network::Network Network;

protected:
	ServerType m_server_type;
	uint16_t m_server_flags;

	uint32_t m_sceneobject_counter;
	fs::FileSystem m_filesystem;
	std::map<std::string, std::shared_ptr<ObjectClass>> m_object_classes;
	std::map<std::string, std::shared_ptr<scene::Tileset>> m_tilesets;
	std::map<std::string, std::shared_ptr<scene::Scene>> m_scenes;
	std::string m_client_script;
};

} // end namespace tdrp::server
