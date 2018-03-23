#include "client/game/Game.h"

#include "engine/loader/LevelLoader.h"
#include "engine/loader/PackageLoader.h"

namespace tdrp
{

/////////////////////////////

Game::Game()
{
	Settings.LoadFromFile("settings.ini");

	Package = loader::PackageLoader::CreatePackage("login");
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
