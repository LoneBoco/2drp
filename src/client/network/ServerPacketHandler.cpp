#include <fstream>

#include "client/network/ServerPacketHandler.h"

#include "client/game/Game.h"
#include "client/render/component/RenderComponent.h"
#include "client/render/component/TileMapRenderComponent.h"
#include "client/render/component/TMXRenderComponent.h"
#include "client/render/component/AnimationRenderComponent.h"
#include "client/render/component/TextRenderComponent.h"
#include "client/loader/ui/UILoader.h"

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
void handle(Game& game, const packet::SceneObjectChunkData& packet);
void handle(Game& game, const packet::SendEvent& packet);
void handle(Game& game, const packet::ItemAdd& packet);
void handle(Game& game, const packet::ItemCount& packet);

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
		case Packets::SCENEOBJECTCHUNKDATA:
			HANDLE(packet::SceneObjectChunkData);
			break;
		case Packets::SENDEVENT:
			HANDLE(packet::SendEvent);
			break;
		case Packets::ITEMADD:
			HANDLE(packet::ItemAdd);
			break;
		case Packets::ITEMCOUNT:
			HANDLE(packet::ItemCount);
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
	// Set our state to loading.
	// The server will process file downloads.
	game.State = GameState::LOADING;

	// Joined UI.
	loader::UILoader::Load(game.UI.get());
	game.UI->LoadContext("loading");
	game.UI->ToggleContextVisibility("loading", true);
	game.UI->ScreenSizeUpdate();
}

void handle(Game& game, const packet::SwitchScene& packet)
{
	const auto& scene_name = packet.scene();

	auto player = game.GetCurrentPlayer();
	if (player)
	{
		auto scene = game.Server.GetOrCreateScene(scene_name);
		player->SwitchScene(scene);
		game.OnSceneSwitch.RunAll(scene);

		log::PrintLine("<- Switched to scene {}.", scene_name);
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
	const auto sceneobject = packet.id();

	if (auto so = game.Server.GetSceneObjectById(sceneobject); so != nullptr)
	{
		// Inform the client we added a scene object.
		if (game.Server.OnSceneObjectAdd != nullptr)
			game.Server.OnSceneObjectAdd(so);
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
		log::PrintLine("<- SceneObjectOwnership [C]: Player {} takes ownership of {} from player {}.", new_player_id, sceneobject_id, old_player_id);
		if (so != nullptr)
			game.OnGainedOwnership.RunAll(so);
	}
}

void handle(Game& game, const packet::SceneObjectChunkData& packet)
{
	const auto sceneobject = packet.id();
	auto so = game.Server.GetSceneObjectById(sceneobject);
	if (!so) return;

	// Currently, only TMX scene objects are supported.
	if (so->GetType() != SceneObjectType::TMX)
		return;

	auto tmx = std::dynamic_pointer_cast<TMXSceneObject>(so);

	// Get the render component.
	auto render = tmx->GetComponent<render::component::TMXRenderComponent>().lock();
	if (render == nullptr)
		return;

	log::PrintLine("<- Loading chunk data for scene object {}.", sceneobject);

	// Load the chunk data.
	// The position and size is recorded by the client and the client will ask the server for tiles when it needs them.
	size_t chunk_idx = packet.index();
	size_t max_chunks = packet.max_count();
	Vector2di chunk_position{ packet.pos_x(), packet.pos_y() };
	Vector2du chunk_dimensions{ packet.width(), packet.height() };
	render->SetMaxChunks(max_chunks);
	tmx->LoadChunkData(chunk_idx, max_chunks, std::move(chunk_position), std::move(chunk_dimensions));

	// TODO: Make this less hacky.
	if (chunk_idx == max_chunks - 1)
		tmx->CalculateLevelSize();

	// Load the collision data.
	tmx->LoadChunkCollision(static_cast<uint32_t>(chunk_idx));

	// If we have tiles, render them.
	if (packet.tiles_size() != 0)
	{
		//std::span<const uint32_t> tiles{ packet.tiles() };
		std::span<const uint32_t> tiles{ packet.tiles().data(), static_cast<size_t>(packet.tiles().size()) };
		render->RenderChunkToTexture(static_cast<uint32_t>(chunk_idx), tiles);
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
		/* TODO: Keep a list of events in Game with a timeout to allow a couple frames of draw.
		sf::CircleShape circle;
		circle.setFillColor(sf::Color::Blue);
		circle.setOutlineColor(sf::Color::Blue);
		circle.setOutlineThickness(1.0f);
		circle.setPosition({ x - radius, y - radius });
		circle.setRadius(radius);
		
		auto window = BabyDI::Get<tdrp::render::Window>();
		window->GetRenderWindow()->resetGLStates();
		window->GetRenderWindow()->draw(circle);
		*/
	}
}

void handle(Game& game, const packet::ItemAdd& packet)
{
	game.UI->MakeItemsDirty();
}

void handle(Game& game, const packet::ItemCount& packet)
{
	game.UI->MakeItemsDirty();
}

} // end namespace tdrp::handlers
