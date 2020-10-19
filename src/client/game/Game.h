#pragma once

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

enum class GameState
{
	INITIALIZING,
	LOADING,
	PLAYING
};

class Game
{
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

	GameState State = GameState::INITIALIZING;

	server::Server Server;
	script::Script Script;

	std::shared_ptr<server::Player> Player;
};

} // end namespace tdrp
