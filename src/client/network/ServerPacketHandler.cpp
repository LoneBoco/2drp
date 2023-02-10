#include <fstream>

#include "client/network/ServerPacketHandler.h"

#include "client/game/Game.h"
#include "client/render/component/RenderComponent.h"
#include "client/render/component/TileMapRenderComponent.h"
#include "client/render/component/TMXRenderComponent.h"
#include "client/render/component/AnimationRenderComponent.h"
#include "client/render/component/TextRenderComponent.h"

#include "engine/scene/SceneObject.h"
#include "engine/network/Packet.h"
#include "engine/network/PacketsInc.h"
#include "engine/network/DownloadManager.h"
#include "engine/filesystem/common.h"

using tdrp::network::Packets;
using tdrp::network::construct;

namespace tdrp::handlers
{

void handle(Game& game, const packet::LoginStatus& packet);
void handle(Game& game, const packet::ServerInfo& packet);
void handle(Game& game, const packet::SwitchScene& packet);
void handle(Game& game, const packet::ClientControlScript& packet);
void handle(Game& game, const packet::ClientScriptAdd& packet);
void handle(Game& game, const packet::ClientScriptDelete& packet);
void handle(Game& game, const packet::SceneObjectNew& packet);
void handle(Game& game, const packet::SceneObjectOwnership& packet);
void handle(Game& game, const packet::SendEvent& packet);

/////////////////////////////

#define HANDLE(cl) handle(game, construct< cl >(packet_data, packet_length))

void network_receive_client(Game& game, const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	// Grab our packet id.
	Packets packet = static_cast<Packets>(packet_id);
	switch (packet)
	{
		case Packets::LOGINSTATUS:
			HANDLE(packet::LoginStatus);
			break;
		case Packets::SERVERINFO:
			HANDLE(packet::ServerInfo);
			break;
		case Packets::SWITCHSCENE:
			HANDLE(packet::SwitchScene);
			break;
		case Packets::CLIENTCONTROLSCRIPT:
			HANDLE(packet::ClientControlScript);
			break;
		case Packets::CLIENTSCRIPTADD:
			HANDLE(packet::ClientScriptAdd);
			break;
		case Packets::CLIENTSCRIPTDELETE:
			HANDLE(packet::ClientScriptDelete);
			break;
		case Packets::SCENEOBJECTNEW:
			HANDLE(packet::SceneObjectNew);
			break;
		case Packets::SCENEOBJECTOWNERSHIP:
			HANDLE(packet::SceneObjectOwnership);
			break;
		case Packets::SENDEVENT:
			HANDLE(packet::SendEvent);
			break;
	}
}

#undef HANDLE

/////////////////////////////

void handle(Game& game, const packet::LoginStatus& packet)
{
	const auto success = packet.success();
	const auto& msg = packet.message();
}

void handle(Game& game, const packet::ServerInfo& packet)
{
	game.State = GameState::LOADING;
}

void handle(Game& game, const packet::SwitchScene& packet)
{
	const auto& scene_name = packet.scene();

	auto player = game.GetCurrentPlayer();
	if (player)
	{
		auto scene = game.Server.GetScene(scene_name);
		player->SwitchScene(scene);
		game.OnSceneSwitch.RunAll(scene);
	}
}

void handle(Game& game, const packet::ClientControlScript& packet)
{
	const auto& script = packet.script();

	log::PrintLine(":: Setting client control script.");
	std::for_each(std::begin(game.BoundScriptFunctions), std::end(game.BoundScriptFunctions), [](tdrp::script::Function* function) { function->Remove("clientcontrol"); });
	game.Script->RunScript("clientcontrol", script, &game);
	game.OnCreated.Run("clientcontrol");
}

void handle(Game& game, const packet::ClientScriptAdd& packet)
{
	const auto& name = packet.name();
	const auto& script = packet.script();

	if (!game.Server.IsHost())
	{
		game.Server.LoadClientScript(name, script);
		game.Server.AddPlayerClientScript(name, game.GetCurrentPlayer());
	}

	log::PrintLine("<- Adding client script {}.", name);
	game.Script->RunScript(name, script, &game);
	game.OnCreated.Run(name);
}

void handle(Game& game, const packet::ClientScriptDelete& packet)
{
	const auto& name = packet.name();

	if (!game.Server.IsHost())
	{
		game.Server.RemovePlayerClientScript(name, game.GetCurrentPlayer());
	}

	log::PrintLine("<- Destroying client script {}.", name);
	game.OnDestroyed.Run(name);
}

void handle(Game& game, const packet::SceneObjectNew& packet)
{
	const auto pid = packet.id();

	auto so = game.Server.GetSceneObjectById(pid);

	// Add the render component.
	if (so)
	{
		switch (so->GetType())
		{
		default:
		case SceneObjectType::STATIC:
			so->AddComponent<render::component::RenderComponent>();
			break;
		case SceneObjectType::TILEMAP:
			so->AddComponent<render::component::TileMapRenderComponent>();
			break;
		case SceneObjectType::TMX:
			so->AddComponent<render::component::TMXRenderComponent>();
			break;
		case SceneObjectType::ANIMATED:
			so->AddComponent<render::component::AnimationRenderComponent>();
			break;
		case SceneObjectType::TEXT:
			so->AddComponent<render::component::TextRenderComponent>();
			break;
		}
	}
}

void handle(Game& game, const packet::SceneObjectOwnership& packet)
{
	const auto sceneobject_id = packet.sceneobject_id();
	const auto old_player_id = packet.old_player_id();
	const auto new_player_id = packet.new_player_id();

	auto so = game.Server.GetSceneObjectById(sceneobject_id);
	auto old_player = game.Server.GetPlayerById(old_player_id);
	auto new_player = game.Server.GetPlayerById(new_player_id);

	if (new_player == game.GetCurrentPlayer())
	{
		// log::PrintLine("<- SceneObjectOwnership [C]: Player {} takes ownership of {} from player {}.", new_player_id, sceneobject_id, old_player_id);
		game.OnGainedOwnership.RunAll(so);
	}
}

void handle(Game& game, const packet::SendEvent& packet)
{
	const auto& sender = packet.sender();
	const auto& pscene = packet.scene();
	const auto& name = packet.name();
	const auto& data = packet.data();
	const auto& x = packet.x();
	const auto& y = packet.y();
	const auto& radius = packet.radius();
	
	auto player = game.GetCurrentPlayer();
	auto scene = game.Server.GetScene(pscene);
	if (player && scene && scene == player->GetCurrentScene().lock())
	{
		// Draw event.
		/*
		sf::CircleShape circle{ radius * 20 };
		circle.setFillColor(sf::Color::Red);
		circle.setPosition({ x, y });
		game.GetRenderWindow()->draw(circle);
		*/

	}
}

} // end namespace tdrp::handlers
