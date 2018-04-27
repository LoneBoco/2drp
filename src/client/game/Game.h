#pragma once

#include "engine/common.h"

#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/ProgramSettings.h"
#include "engine/network/Network.h"
#include "engine/package/Package.h"

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

	void Update();

	settings::ProgramSettings Settings;
	network::Network Network;
	std::shared_ptr<package::Package> Package;

private:

	void network_connect(const uint16_t id, ENetEvent& event);
	void network_disconnect(const uint16_t id, ENetEvent& event);
	void network_receive(const uint16_t id, ENetEvent& event);

};

} // end namespace tdrp
