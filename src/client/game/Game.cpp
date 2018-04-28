#include "client/game/Game.h"

#include "client/network/PacketHandler.h"

#include "engine/loader/LevelLoader.h"
#include "engine/loader/PackageLoader.h"

namespace tdrp
{

/////////////////////////////

Game::Game()
{
	if (!network::Network::Startup())
		throw std::exception("ENet failed to initialize!");

	Settings.LoadFromFile("settings.ini");

	Package = loader::PackageLoader::CreatePackage("login");
	if (Package)
	{
		auto scene = loader::LevelLoader::CreateScene(*Package, "startlevel");
	}

	using namespace std::placeholders;
	Network.SetConnectCallback(std::bind(handlers::network_connect, std::ref(*this), _1));
	Network.SetDisconnectCallback(std::bind(handlers::network_disconnect, std::ref(*this), _1));
	Network.SetReceiveCallback(std::bind(handlers::network_receive, std::ref(*this), _1, _2, _3, _4));
}

Game::~Game()
{
	network::Network::Shutdown();
}

void Game::Update()
{
	if (Package)
	{
		Package->GetFileSystem().Update();
	}

	Network.Update();
}

/////////////////////////////

} // end namespace tdrp
