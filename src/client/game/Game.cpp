#include "client/game/Game.h"

namespace tdrp
{

/////////////////////////////

Game::Game()
{
	auto settings_file = FileSystem.GetFile("settings.ini");
	if (settings_file != nullptr)
		Settings.LoadFromFile(*settings_file);

	// FileSystem.LoadZipFiles("packages");
}

/////////////////////////////

} // end namespace tdrp
