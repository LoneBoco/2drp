#pragma once

#include <future>

#include "engine/common.h"

#include "engine/package/Package.h"
#include "engine/scene/Scene.h"

#include "engine/server/Account.h"

#include <google/protobuf/message.h>

struct _ENetHost;
struct _ENetPeer;
struct _ENetPacket;
struct enet_host_deleter;

namespace tdrp::server
{
	class Server;
	class Player;
	using PlayerPtr = std::shared_ptr<Player>;
}

namespace tdrp::network
{

struct enet_host_deleter
{
	void operator()(_ENetHost* host) const;
};

//! Peer ID
typedef std::function<void(const uint16_t)> enet_connection_cb;

//! Peer ID, Packet ID, Packet Data, Packet Length
typedef std::function<void(const uint16_t, const uint16_t, const uint8_t* const, const size_t)> enet_receive_cb;

enum class Channel
{
	UNRELIABLE = 0,
	RELIABLE,
	FILE,

	COUNT
};

class Network
{
public:
	Network();
	~Network();

	Network(const Network& other) = delete;
	Network(Network&& other) = delete;
	Network& operator=(const Network& other) = delete;
	Network& operator=(Network&& other) = delete;

	void BindServer(server::Server* server);

	static bool Startup();
	static void Shutdown();
	bool Initialize(const size_t peers = 1, const uint16_t port = 0);

public:
	std::future<bool> Connect(const std::string& hostname, const uint16_t port);
	void Disconnect();
	void DisconnectPeer(const uint16_t peer_id);

public:
	void Update(bool isServer = false);

public:
	void Send(const uint16_t peer_id, const uint16_t packet_id, const Channel channel);
	void Send(const uint16_t peer_id, const uint16_t packet_id, const Channel channel, google::protobuf::Message& message);
	void Broadcast(const uint16_t packet_id, const Channel channel);
	void Broadcast(const uint16_t packet_id, const Channel channel, google::protobuf::Message& message);
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, uint16_t packet_id, const Channel channel);
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, google::protobuf::Message& message);
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel);
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, google::protobuf::Message& message);

protected:
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, _ENetPacket* packet);
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, _ENetPacket* packet);

public:
	bool BindAccountToPeer(const uint16_t peer_id, std::unique_ptr<server::Account>&& account);
	server::Account* GetAccountFromPeer(const uint16_t peer_id);

private:
	_ENetPacket* construct_packet(const Channel channel, const uint16_t packet_id, google::protobuf::Message* message = nullptr);

public:
	void SetConnectCallback(enet_connection_cb callback) { m_connect_cb = callback; }
	void SetDisconnectCallback(enet_connection_cb callback) { m_disconnect_cb = callback; }
	void SetLoginCallback(enet_receive_cb callback) { m_login_cb = callback; }
	void SetReceiveCallback(enet_receive_cb callback) { m_receive_cb = callback; }

private:
	static bool ms_started;
	server::Server* m_server;

	enet_connection_cb m_connect_cb;
	enet_connection_cb m_disconnect_cb;
	enet_receive_cb m_login_cb;
	enet_receive_cb m_receive_cb;
	std::unique_ptr<_ENetHost, enet_host_deleter> m_host;
	std::map<uint16_t, _ENetPeer*> m_peers;
};

} // end namespace tdrp::network
