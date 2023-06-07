#pragma once

#include <unordered_map>

#include "client/game/Camera.h"
#include "client/ui/UIManager.h"

#include "engine/common.h"

#include "engine/filesystem/FileSystem.h"
#include "engine/network/Network.h"
#include "engine/package/Package.h"
#include "engine/server/Server.h"
#include "engine/server/Player.h"
#include "engine/script/Script.h"
#include "engine/item/Item.h"

namespace sf
{
	class Sound;
	class RenderWindow;
	class RenderTarget;
}

namespace tdrp
{

// constexpr std::chrono::milliseconds PROP_UPDATE_TIMER = 30ms;
using ItemList = std::set<item::ItemInstancePtr>;


enum class GameState
{
	INITIALIZING,
	LOADING,
	PLAYING
};

class Game
{
	SCRIPT_SETUP;
	SCRIPT_FUNCTION(OnCreated);
	SCRIPT_FUNCTION(OnDestroyed);
	SCRIPT_FUNCTION(OnConnected);
	SCRIPT_FUNCTION(OnClientFrame);
	SCRIPT_FUNCTION(OnKeyPress);
	SCRIPT_FUNCTION(OnMouseWheel);
	SCRIPT_FUNCTION(OnMouseDown);
	SCRIPT_FUNCTION(OnMouseUp);
	SCRIPT_FUNCTION(OnGainedOwnership);
	SCRIPT_FUNCTION(OnSceneSwitch);

public:
	Game();
	~Game();

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game& operator=(Game&& other) = delete;

	void Initialize();

public:
	void Update();
	void Render(sf::RenderWindow* window);

public:
	chrono::clock::duration GetTick() const;

public:
	void SendEvent(const std::string& name, const std::string& data, Vector2df origin, float radius);
	void SendLevelEvent(const std::string& name, const std::string& data);
	void SendServerEvent(const std::string& name, const std::string& data);

public:
	GameState State = GameState::INITIALIZING;
	server::Server Server;
	camera::Camera Camera;
	script::ScriptPtr Script;
	ui::UIManagerPtr UI;

public:
	server::PlayerPtr GetCurrentPlayer();

public:
	std::list<std::shared_ptr<sf::Sound>> PlayingSounds;

private:
	chrono::clock::time_point m_tick_previous;
	chrono::clock::time_point m_tick_current;

	sf::RenderWindow* m_render_window = nullptr;
};

inline chrono::clock::duration Game::GetTick() const
{
	return m_tick_current - m_tick_previous;
}

inline server::PlayerPtr Game::GetCurrentPlayer()
{
	return Server.GetPlayer();
}

} // end namespace tdrp
