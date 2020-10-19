#include <iostream>

#include <SFML/Graphics.hpp>

#include "client/game/Game.h"

#include "client/render/component/RenderComponent.h"
#include "client/network/DownloadManager.h"
#include "client/network/ServerPacketHandler.h"
#include "client/script/Script.h"

#include "engine/filesystem/ProgramSettings.h"
#include "engine/loader/LevelLoader.h"
#include "engine/loader/PackageLoader.h"
#include "engine/network/Packet.h"


namespace tdrp
{

/////////////////////////////

Game::Game()
{
	using namespace std::placeholders;
	Server.SetClientNetworkReceiveCallback(std::bind(handlers::network_receive, std::ref(*this), _1, _2, _3, _4));

	if (!Server.Initialize("login", server::ServerType::AUTHORITATIVE, FLAGS<uint16_t>(server::ServerFlags::PRELOAD_EVERYTHING)))
		throw std::runtime_error("Unable to start the server.");
}

void Game::Initialize()
{
	// Bind game script classes.
	bind_game(Script.GetLuaState());

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
	else
	{
		Server.SinglePlayer();
	}
}

Game::~Game()
{
}

void Game::Update()
{
	Server.Update();

	if (State == GameState::LOADING)
	{
		BabyDI::Injected<tdrp::DownloadManager> downloader;
		if (!downloader->FilesInQueue && !Server.FileSystem.IsSearchingForFiles() && Server.GetPackage())
		{
			State = GameState::PLAYING;
			
			// Set our player.
			Player = Server.GetPlayerById(0);

			// Send the finished loading packet.
			Server.Send(0, network::PACKETID(network::ClientPackets::READY), network::Channel::RELIABLE);
		}
	}
	else if (State == GameState::PLAYING)
	{
		// TODO: Run the client frame tick script.
	}
}

void Game::Render(sf::RenderWindow* window)
{
	if (Player)
	{
		if (auto scene = Player->GetCurrentScene().lock())
		{
			// TODO: Proper camera handling.
			auto within_camera = scene->FindObjectsInRangeOf({ 0.0f, 0.0f }, 10000.0f);

			// Sort by Z, then by Y, then by X.
			std::stable_sort(std::begin(within_camera), std::end(within_camera), [](const decltype(within_camera)::value_type& a, const decltype(within_camera)::value_type& b) -> bool
			{
				if (a->GetDepth() < b->GetDepth()) return true;
				if (b->GetDepth() < a->GetDepth()) return false;

				auto a_pos = a->GetPosition();
				auto b_pos = b->GetPosition();

				if (a_pos.y < b_pos.y) return true;
				if (b_pos.y < a_pos.y) return false;

				if (a_pos.x < b_pos.x) return true;
				if (b_pos.x < a_pos.x) return false;

				return false;
			});

			// Render!
			for (const auto& so : within_camera)
			{
				auto comp = so->GetComponent<render::component::RenderComponent>();
				if (auto render = comp.lock())
				{
					render->Render(*window);
				}
			}
		}
	}
}

/////////////////////////////

} // end namespace tdrp
