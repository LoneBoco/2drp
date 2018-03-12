#include "client/game/Game.h"

#include "engine/loader/LevelLoader.h"

namespace tdrp
{

/////////////////////////////

Game::Game()
{
	Settings.LoadFromFile("settings.ini");
	Package = std::make_shared<package::Package>("login");

	if (Package)
	{
		auto scene = loader::LevelLoader::CreateScene(*Package, "startlevel");
	}
}

void Game::Update()
{
	if (Package)
	{
		Package->GetFileSystem().Update();
	}
}

/////////////////////////////

} // end namespace tdrp
