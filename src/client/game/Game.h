#pragma once

#include "engine/common.h"

#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/ProgramSettings.h"

namespace tdrp
{

class Game
{
public:
	Game();
	~Game() = default;

	void Update();

	fs::FileSystem FileSystem;
	settings::ProgramSettings Settings;

private:

};

} // end namespace tdrp
