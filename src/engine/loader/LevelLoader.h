#pragma once

#include "engine/common.h"
#include "engine/package/Package.h"
#include "engine/scene/Scene.h"

namespace tdrp::loader
{

class LevelLoader
{
public:
	LevelLoader() = default;
	~LevelLoader() = default;

	LevelLoader(const LevelLoader& other) = delete;
	LevelLoader(LevelLoader&& other) = delete;
	LevelLoader& operator=(const LevelLoader& other) = delete;
	LevelLoader& operator=(LevelLoader&& other) = delete;

	static std::shared_ptr<tdrp::scene::Scene> CreateScene(package::Package& package, const std::string& level);

private:
};

} // end namespace tdrp::loader
