#include "engine/server/Server.h"
#include "engine/loader/PackageLoader.h"
#include "engine/loader/LevelLoader.h"
#include "engine/filesystem/File.h"

namespace tdrp::server
{

Server::Server()
	: m_sceneobject_counter(0)
{
	// Create blank object class.
	m_object_classes.insert(std::make_pair("blank", std::make_shared<ObjectClass>("blank")));
}

bool Server::Initialize(const std::string& package_name, const ServerType type, const uint16_t flags)
{
	m_server_type = type;
	m_server_flags = flags;

	auto[load_success, package] = loader::PackageLoader::LoadIntoServer(*this, package_name);

	// Load everything.
	if (m_server_flags & static_cast<uint16_t>(ServerFlags::PRELOAD_EVERYTHING))
	{
		// Load all scenes.
		for (auto& d : filesystem::directory_iterator{ package->GetBasePath() / "levels" })
		{
			if (!filesystem::is_directory(d.status()))
				continue;

			auto scene = loader::LevelLoader::CreateScene(*this, d);
			m_scenes.insert(std::make_pair(d.path().filename().string(), scene));
		}
	}

	// If we are not a single player only server, start up the network.
	if (~m_server_flags & static_cast<uint16_t>(ServerFlags::SINGLEPLAYER_ONLY))
	{
		if (!network::Network::Startup())
			return false;
	}

	return true;
}

void Server::Update()
{
	Network.Update();
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

} // end namespace tdrp::server
