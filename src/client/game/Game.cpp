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
	Server.GetNetwork().AddReceiveCallback(std::bind(handlers::network_receive_client, std::ref(*this), _1, _2, _3, _4));

	auto settings = BabyDI::Get<tdrp::settings::ProgramSettings>();
	std::string package{ settings->Get("game.package", "login") };

	// Calculate the type of server to initialize.
	auto server_type = server::ServerType::HOST;
	uint16_t flags = 0;
	if (settings->GetAs<bool>("game.networked"))
	{
		if (settings->GetAs<bool>("game.hosting", false))
			flags = FLAGS<uint16_t>(server::ServerFlags::PRELOAD_EVERYTHING);
		else
			server_type = server::ServerType::GUEST;
	}
	else flags = FLAGS<uint16_t>(server::ServerFlags::SINGLEPLAYER);

	// Initialize the server.
	if (!Server.Initialize(package, server_type, flags))
		throw std::runtime_error("Unable to start the server.");
}

void Game::Initialize()
{
	// Bind game script classes.
	Script->BindIntoMe(
		&bind_client,
		[this](sol::state& lua) { lua["Server"] = &this->Server; }
	);

	// Load the UI.
	UI = loader::UILoader::CreateUIManager();
	UI->RenderInterface->SetWindow(m_render_window);
	UI->ScreenSizeUpdate();

	// Load settings.
	auto settings = BabyDI::Get<tdrp::settings::ProgramSettings>();
	if (settings->Exists("game.networked"))
	{
		if (settings->Exists("game.hosting"))
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
		else throw "** ERROR: Networked game but cannot determine if host or guest.";
	}
	else
	{
		Server.SinglePlayer();
		State = GameState::PLAYING;

		// Register client scripts.
		/*
		for (const auto& [name, script] : Server.GetClientScriptMap())
		{
			Script->RunScript(name, script, this);
			OnCreated.Run(name);
		}
		*/

		// Call connected callback.
		OnConnected.RunAll();

		// Call the OnPlayerJoin script function.
		Server.OnPlayerJoin.RunAll(GetCurrentPlayer());
	}
}

Game::~Game()
{
	log::PrintLine(":: Shutting down game.");

	Server.Shutdown();

	// Erase script instance.
	SCRIPT_ME_ERASE;

	// Erase our script instance.
	auto script_manager = BabyDI::Get<script::ScriptManager>();
	script_manager->EraseScriptInstance("Game");
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

	if (State == GameState::PLAYING)
	{
		// Run the client frame tick script.
		OnClientFrame.RunAll(tick_in_ms.count());
	}

	// Update the server last so attributes are sent after all scripts are done.
	Server.Update(tick_in_ms);

	// Update camera after everything has been drawn.
	Camera.Update(tick_in_ms);

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

void Game::SendEvent(SceneObject* sender, const std::string& name, const std::string& data, Vector2df origin, float radius)
{
	Server.SendEvent(GetCurrentPlayer()->GetCurrentScene().lock(), sender, name, data, origin, radius);
}

useable::UseablePtr Game::CreateUseable(const std::string& name, const std::string& image, const std::string& description)
{
	auto it = std::ranges::find_if(m_useables, [&name](useable::UseablePtr& u) { return u->Name == name; });
	if (it != std::end(m_useables))
	{
		log::PrintLine("!! Useable {} already exists, overwriting.", name);
		m_useables.erase(it);
	}

	auto& useable = m_useables.emplace_back(std::make_shared<useable::Useable>(name, image, description));

	UI->MakeUseablesDirty();

	return useable;
}

void Game::DeleteUseable(const std::string& name)
{
	auto it = std::ranges::find_if(m_useables, [&name](useable::UseablePtr& u) { return u->Name == name; });
	if (it != std::end(m_useables))
	{
		log::PrintLine(":: Deleting useable {}.", name);
		m_useables.erase(it);

		UI->MakeUseablesDirty();
	}
	else
	{
		log::PrintLine(":: Could not find useable {} to delete.", name);
	}
}

void Game::CallUseable(const std::string& name)
{
	auto it = std::ranges::find_if(m_useables, [&name](useable::UseablePtr& u) { return u->Name == name; });
	if (it != std::end(m_useables))
	{
		(*it)->OnUsed.RunAll();
	}
}

/////////////////////////////

} // end namespace tdrp
