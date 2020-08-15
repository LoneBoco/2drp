#pragma once

#include "engine/common.h"

#include "engine/filesystem/FileSystem.h"
#include "engine/network/Network.h"
#include "engine/package/Package.h"
#include "engine/server/Server.h"
#include "engine/server/Player.h"
#include "engine/script/Script.h"

namespace tdrp
{

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

	server::Server Server;
	script::Script Script;

	std::unique_ptr<fs::FileSystem> Filesystem;

	std::shared_ptr<server::Player> Player;
};

} // end namespace tdrp
