#include <enet/enet.h>

#include "client/game/Game.h"

#include "engine/network/PacketID.h"
#include "engine/network/packets/SError.pb.h"

#include "engine/loader/LevelLoader.h"
#include "engine/loader/PackageLoader.h"

namespace tdrp
{

/////////////////////////////

Game::Game()
{
	int ret = enet_initialize();
	if (ret != 0)
		throw std::exception("ENet failed to initialize!");

	Settings.LoadFromFile("settings.ini");

	Package = loader::PackageLoader::CreatePackage("login");
	if (Package)
	{
		auto scene = loader::LevelLoader::CreateScene(*Package, "startlevel");
	}

	using std::placeholders::_1;
	using std::placeholders::_2;
	Network.SetConnectCallback(std::bind(&Game::network_connect, this, _1, _2));
	Network.SetDisconnectCallback(std::bind(&Game::network_disconnect, this, _1, _2));
	Network.SetReceiveCallback(std::bind(&Game::network_receive, this, _1, _2));
}

Game::~Game()
{
	enet_deinitialize();
}

void Game::Update()
{
	if (Package)
	{
		Package->GetFileSystem().Update();
	}

	Network.Update();
}

void Game::network_connect(const uint16_t id, ENetEvent& event)
{
}

void Game::network_disconnect(const uint16_t id, ENetEvent& event)
{
}

void Game::network_receive(const uint16_t id, ENetEvent& event)
{
	if (event.packet->dataLength < 2)
		return;

	// Grab our packet id.
	ServerPackets packet = static_cast<ServerPackets>(event.packet->data[0] & 0xFF | event.packet->data[1] << 8);
	switch (packet)
	{
		case ServerPackets::ERROR:
			packet::SError s_error;
			s_error.ParseFromArray(event.packet->data + 2, event.packet->dataLength - 2);
			break;
	}
}

/////////////////////////////

} // end namespace tdrp
