#include <iostream>

#include "client/game/Game.h"
#include "client/network/ServerPacketHandler.h"
#include "client/script/Script.h"

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

	if (!Server.Initialize("login", server::ServerType::AUTHORITATIVE, FLAGS<uint16_t>(server::ServerFlags::PRELOAD_EVERYTHING)))
		throw std::runtime_error("Unable to start the server.");
}

void Game::Initialize()
{
	/*
	BabyDI::Injected<tdrp::settings::ProgramSettings> settings;
	if (settings->Exists("game.starthosting"))
	{
		uint16_t port = settings->GetAs<uint16_t>("network.port");
		Server.Host(port);
	}
	else if (settings->Exists("network.server"))
	{
		std::string host{ settings->Get("network.server") };
		uint16_t port = settings->GetAs<uint16_t>("network.port");
		Server.Connect(host, port);
	}
	*/

	// Bind game script classes.
	bind_game(Script.GetLuaState());

	// Set the server as single player.
	Server.SinglePlayer();
}

Game::~Game()
{
	network::Network::Shutdown();
}

void Game::Update()
{
	// TODO: Run the client frame tick script.

	Server.Update();

	if (Filesystem != nullptr)
		Filesystem->Update();
}

/////////////////////////////

} // end namespace tdrp
