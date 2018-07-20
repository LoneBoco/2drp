#include <iostream>

#include "client/game/Game.h"
#include "client/network/PacketHandler.h"

#include "engine/filesystem/ProgramSettings.h"
#include "engine/loader/LevelLoader.h"
#include "engine/loader/PackageLoader.h"

namespace tdrp
{

/////////////////////////////

Game::Game()
{
	using namespace std::placeholders;
	Server.Network.SetReceiveCallback(std::bind(handlers::network_receive, std::ref(*this), _1, _2, _3, _4));

	if (!Server.Initialize("login", server::ServerType::AUTHORITATIVE, static_cast<uint16_t>(server::ServerFlags::PRELOAD_EVERYTHING)))
		throw std::runtime_error("Unable to start the server.");
}

void Game::Initialize()
{
	BabyDI::Injected<tdrp::settings::ProgramSettings> settings;
	if (settings->Exists("game.starthosting"))
	{
		uint16_t port = settings->GetAs<uint16_t>("network.port");
		std::cout << "hosting on port " << port << std::endl;
		Server.Host(port);
	}
	else if (settings->Exists("network.server"))
	{
		std::string host{ settings->Get("network.server") };
		uint16_t port = settings->GetAs<uint16_t>("network.port");
		std::cout << "connecting to " << host << " on port " << port << std::endl;
		Server.Connect(host, port);
	}
}

Game::~Game()
{
	network::Network::Shutdown();
}

void Game::Update()
{
	Server.Update();
}

/////////////////////////////

} // end namespace tdrp
