#include <iostream>

#include <SFML/Graphics.hpp>

#include "client/game/Game.h"

#include "client/render/component/RenderComponent.h"
#include "client/network/DownloadManager.h"
#include "client/network/ServerPacketHandler.h"
#include "client/script/Script.h"

#include "engine/filesystem/File.h"
#include "engine/filesystem/ProgramSettings.h"
#include "engine/loader/LevelLoader.h"
#include "engine/loader/PackageLoader.h"
#include "engine/network/Packet.h"

#include "engine/network/PacketsClient.h"
#include "engine/network/packets/CSceneObjectChange.pb.h"
#include "engine/network/packets/CSendEvent.pb.h"


namespace tdrp
{

/////////////////////////////

Game::Game()
{
	m_tick_current = chrono::clock::now();

	using namespace std::placeholders;
	Server.SetClientNetworkReceiveCallback(std::bind(handlers::network_receive, std::ref(*this), _1, _2, _3, _4));

	auto settings = BabyDI::Get<tdrp::settings::ProgramSettings>();
	std::string package{ settings->Get("game.package", "login") };

	if (!Server.Initialize(package, server::ServerType::AUTHORITATIVE, FLAGS<uint16_t>(server::ServerFlags::PRELOAD_EVERYTHING)))
		throw std::runtime_error("Unable to start the server.");
}

void Game::Initialize()
{
	// Bind game script classes.
	bind_globals(Script.GetLuaState());
	bind_game(Script.GetLuaState());
	bind_camera(Script.GetLuaState());

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
	}
}

Game::~Game()
{
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
		auto downloader = BabyDI::Get<tdrp::DownloadManager>();
		if (!downloader->FilesInQueue && !Server.FileSystem.IsSearchingForFiles() && Server.GetPackage())
		{
			State = GameState::PLAYING;
			
			// Set our player.
			Player = Server.GetPlayerById(0);

			// Send the finished loading packet.
			Server.Send(0, network::PACKETID(network::ClientPackets::READY), network::Channel::RELIABLE);

			// Run our OnConnected callback.
			OnConnected.RunAll<Game>();
		}
	}
	else if (State == GameState::PLAYING)
	{
		// Run the client frame tick script.
		OnClientFrame.RunAll<Game>(tick_in_ms.count());

		// Send prop updates for NPCs we control.
		if (Player)
		{
			if (auto so = Player->GetCurrentControlledSceneObject().lock())
			{
				m_prop_update_timer += tick_in_ms;
				if (m_prop_update_timer > PROP_UPDATE_TIMER)
				{
					m_prop_update_timer = std::chrono::milliseconds::zero();

					if (so->Properties.HasDirty() || so->Attributes.HasDirty())
					{
						auto packet = getPropsPacket<packet::CSceneObjectChange>(*so);
						packet.set_id(so->ID);
						Server.Send(0, network::PACKETID(network::ClientPackets::SCENEOBJECTCHANGE), network::Channel::RELIABLE);
					}
				}
			}
		}
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
	if (Player)
	{
		if (auto scene = Player->GetCurrentScene().lock())
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
	packet::CSendEvent packet;
	packet.set_sender(sender ? sender->ID : 0);
	packet.set_name(name);
	packet.set_data(data);
	packet.set_x(origin.x);
	packet.set_y(origin.y);
	packet.set_radius(radius);

	Server.Send(0, network::PACKETID(network::ClientPackets::SENDEVENT), network::Channel::RELIABLE, packet);
}

/////////////////////////////

} // end namespace tdrp
