#include "client/game/game.h"

namespace tdrp
{

/////////////////////////////

Game::Game()
{
	auto settings_file = FileSystem.GetFile("settings.ini");
	if (settings_file != nullptr)
		Settings.LoadFromFile(*settings_file);
}

/////////////////////////////

} // end namespace tdrp
