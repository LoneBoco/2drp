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
		throw std::runtime_error("ENet failed to initialize!");

	/*
	Package = loader::PackageLoader::CreatePackage("login");
	if (Package)
	{
		auto scene = loader::LevelLoader::CreateScene(*Package, "startlevel");
	}
	*/

	using namespace std::placeholders;
	Server.Network.SetConnectCallback(std::bind(handlers::network_connect, std::ref(*this), _1));
	Server.Network.SetDisconnectCallback(std::bind(handlers::network_disconnect, std::ref(*this), _1));
	Server.Network.SetReceiveCallback(std::bind(handlers::network_receive, std::ref(*this), _1, _2, _3, _4));

	Server.Initialize("login", server::ServerType::AUTHORITATIVE, static_cast<uint16_t>(server::ServerFlags::PRELOAD_EVERYTHING));
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

	Server.Update();
}

/////////////////////////////

} // end namespace tdrp
