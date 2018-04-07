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

	static std::shared_ptr<tdrp::scene::Scene> CreateScene(package::Package& package, const std::string& level);

private:
};

} // end namespace tdrp::loader
