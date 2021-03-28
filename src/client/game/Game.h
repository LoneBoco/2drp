#pragma once

#include "client/game/Camera.h"

#include "engine/common.h"

#include "engine/filesystem/FileSystem.h"
#include "engine/network/Network.h"
#include "engine/package/Package.h"
#include "engine/server/Server.h"
#include "engine/server/Player.h"
#include "engine/script/Script.h"

namespace sf
{
	class RenderWindow;
}

namespace tdrp
{

constexpr std::chrono::milliseconds PROP_UPDATE_TIMER = 30ms;


enum class GameState
{
	INITIALIZING,
	LOADING,
	PLAYING
};

class Game
{
	SCRIPT_FUNCTION(OnCreated);
	SCRIPT_FUNCTION(OnConnected);
	SCRIPT_FUNCTION(OnClientFrame);
	SCRIPT_FUNCTION(OnControlledActorChange);
	SCRIPT_FUNCTION(OnSceneSwitch);
	SCRIPT_ENVIRONMENT;

public:
	Game();
	~Game();

	Game(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game& operator=(Game&& other) = delete;

	void Initialize();

	void Update();
	void Render(sf::RenderWindow* window);

	chrono::clock::duration GetTick() const;

	GameState State = GameState::INITIALIZING;

	server::Server Server;
	script::Script Script;
	camera::Camera Camera;
	std::shared_ptr<server::Player> Player;

private:
	chrono::clock::time_point m_tick_previous;
	chrono::clock::time_point m_tick_current;
	std::chrono::milliseconds m_prop_update_timer = std::chrono::milliseconds::zero();
};

inline chrono::clock::duration Game::GetTick() const
{
	return m_tick_current - m_tick_previous;
}

} // end namespace tdrp
