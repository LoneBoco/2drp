#pragma once

#include "engine/common.h"

#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/ProgramSettings.h"
#include "engine/package/Package.h"

namespace tdrp
{

class Game
{
public:
	Game();
	~Game() = default;

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game& operator=(Game&& other) = delete;

	void Update();

	settings::ProgramSettings Settings;
	std::shared_ptr<package::Package> Package;

private:

};

} // end namespace tdrp
