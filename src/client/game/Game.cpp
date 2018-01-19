#include "client/game/Game.h"

namespace tdrp
{

/////////////////////////////

Game::Game()
{
	Settings.LoadFromFile("settings.ini");
}

void Game::Update()
{
	FileSystem.Update();
}

/////////////////////////////

} // end namespace tdrp
