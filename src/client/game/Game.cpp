#include <iostream>

#include <SFML/Graphics.hpp>
#include <RmlUi/Core.h>

#include "client/game/Game.h"

#include "client/render/component/RenderComponent.h"
#include "client/network/ServerPacketHandler.h"
#include "client/script/Script.h"
#include "client/loader/ui/UILoader.h"

#include "engine/filesystem/File.h"
#include "engine/filesystem/ProgramSettings.h"
#include "engine/loader/LevelLoader.h"
#include "engine/loader/PackageLoader.h"
#include "engine/network/Packet.h"
#include "engine/network/DownloadManager.h"

#include "engine/network/PacketsInc.h"
#include "engine/network/packets/SceneObjectChange.pb.h"
#include "engine/network/packets/SendEvent.pb.h"


namespace tdrp
{

/////////////////////////////

Game::Game()
{
	m_tick_current = chrono::clock::now();

	auto script_manager = BabyDI::Get<script::ScriptManager>();
	Script = script_manager->CreateScriptInstance("Game");

	using namespace std::placeholders;
	//Server.SetClientNetworkReceiveCallback(std::bind(handlers::network_receive, std::ref(*this), _1, _2, _3, _4));
	Server.GetNetwork().AddReceiveCallback(std::bind(handlers::network_receive_client, std::ref(*this), _1, _2, _3, _4));

	auto settings = BabyDI::Get<tdrp::settings::ProgramSettings>();
	std::string package{ settings->Get("game.package", "login") };

	if (!Server.Initialize(package, server::ServerType::HOST, FLAGS<uint16_t>(server::ServerFlags::PRELOAD_EVERYTHING)))
		throw std::runtime_error("Unable to start the server.");
}

void Game::Initialize()
{
	// Bind game script classes.
	Script->BindIntoMe(
		&bind_globals,
		&bind_game,
		&bind_camera
	);

	// Load the UI.
	UI = loader::UILoader::CreateUIManager();
	UI->RenderInterface->SetWindow(m_render_window);
	UI->ScreenSizeUpdate();

	// Load settings.
	auto settings = BabyDI::Get<tdrp::settings::ProgramSettings>();
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
		State = GameState::PLAYING;

		// Register client scripts.
		for (const auto& [name, script] : Server.GetClientScriptMap())
		{
			Script->RunScript(name, script, *this);
			OnCreated.Run(name);
		}

		// Call connected callback.
		OnConnected.RunAll();

		// Call the OnPlayerJoin script function.
		Server.OnPlayerJoin.RunAll(GetCurrentPlayer());
	}
}

Game::~Game()
{
	log::PrintLine(":: Shutting down game.");
}

void Game::Update()
{
	// Time tick.
	m_tick_previous = m_tick_current;
	m_tick_current = chrono::clock::now();

	auto tick_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(GetTick());

	Server.PreUpdate();

	if (State == GameState::LOADING)
	{
		if (!Server.DownloadManager.FilesInQueue && !Server.FileSystem.IsSearchingForFiles() && Server.GetPackage())
		{
			State = GameState::PLAYING;
			
			// Set our player.
			//Player = Server.GetPlayerById(0);

			// Send the finished loading packet.
			Server.Send(0, network::PACKETID(network::Packets::CLIENTREADY), network::Channel::RELIABLE);

			// Run our OnConnected callback.
			OnConnected.RunAll();
		}
	}
	else if (State == GameState::PLAYING)
	{
		// Run the client frame tick script.
		OnClientFrame.RunAll(tick_in_ms.count());
	}

	// Update the server last so attributes are sent after all scripts are done.
	Server.Update(GetTick());

	// Update camera after everything has been drawn.
	Camera.Update(GetTick());

	// Remove any sounds that have finished playing.
	std::erase_if(PlayingSounds,
		[](decltype(PlayingSounds)::const_reference sound) -> bool
		{
			if (sound->getStatus() == sf::SoundSource::Stopped)
				return true;
			return false;
		}
	);
}

void Game::Render(sf::RenderWindow* window)
{
	auto player = GetCurrentPlayer();
	if (player)
	{
		if (auto scene = player->GetCurrentScene().lock())
		{
			// auto window_size = std::max(Camera.GetViewWindow().size.x, Camera.GetViewWindow().size.y) * 2.0f;
			// auto within_camera = scene->FindObjectsInRangeOf(Vector2df{ Camera.GetViewWindow().pos }, window_size);
			auto within_camera = scene->FindObjectsBoundInRectangle(Camera.GetViewRect());

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
				auto comp = so->GetComponentDerivedFrom<render::component::IRenderableComponent>();
				if (auto render = comp.lock())
				{
					render->Render(*window, std::chrono::duration_cast<std::chrono::milliseconds>(GetTick()));
				}
			}
		}
	}
}

void Game::SendEvent(std::shared_ptr<SceneObject> sender, const std::string& name, const std::string& data, Vector2df origin, float radius)
{
	Server.SendEvent(GetCurrentPlayer()->GetCurrentScene().lock(), sender, name, data, origin, radius);
}

/////////////////////////////

} // end namespace tdrp
