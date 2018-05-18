#pragma once

#include "engine/common.h"
#include "engine/server/Server.h"
#include "engine/scene/Scene.h"

namespace tdrp::loader
{

class LevelLoader
{
public:
	LevelLoader() = delete;
	~LevelLoader() = delete;

	LevelLoader(const LevelLoader& other) = delete;
	LevelLoader(LevelLoader&& other) = delete;
	LevelLoader& operator=(const LevelLoader& other) = delete;
	LevelLoader& operator=(LevelLoader&& other) = delete;

	static std::shared_ptr<tdrp::scene::Scene> CreateScene(server::Server& server, const filesystem::path& level);

private:
};

} // end namespace tdrp::loader
