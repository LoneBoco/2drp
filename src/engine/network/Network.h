#pragma once

#include <future>
#include <deque>

#include "engine/common.h"

#include "engine/package/Package.h"
#include "engine/scene/Scene.h"

#include "engine/server/Account.h"
#include "engine/server/Player.h"

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

constexpr int HOSTID = 0;

struct enet_host_deleter
{
	void operator()(_ENetHost* host) const;
};

//! Peer ID
typedef std::function<void(const uint16_t)> enet_connection_cb;

//! Peer ID, Packet ID, Packet Data, Packet Length
typedef std::function<void(const uint16_t, const uint16_t, const uint8_t* const, const size_t)> enet_receive_cb;

using enet_connection_cb_container = std::deque<enet_connection_cb>;
using enet_receive_cb_container = std::deque<enet_receive_cb>;

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
	static bool IsStarted() { return ms_started; }

	bool Initialize(const size_t peers = 1, const uint16_t port = 0);

public:
	std::future<bool> Connect(const std::string& hostname, const uint16_t port);
	void Disconnect();
	void DisconnectPeer(const uint16_t player_id);

public:
	void Update();

public:
	void Send(const uint16_t player_id, const uint16_t packet_id, const Channel channel);
	void Send(const uint16_t player_id, const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message);
	void Broadcast(const uint16_t packet_id, const Channel channel);
	void Broadcast(const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message);
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, uint16_t packet_id, const Channel channel, const std::set<server::PlayerPtr>& exclude = {});
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message, const std::set<server::PlayerPtr>& exclude = {});
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, const std::set<server::PlayerPtr>& exclude = {});
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message, const std::set<server::PlayerPtr>& exclude = {});

protected:
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, _ENetPacket* packet, const std::set<server::PlayerPtr>& exclude = {});
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, _ENetPacket* packet, const std::set<server::PlayerPtr>& exclude = {});

private:
	_ENetPacket* construct_packet(const Channel channel, const uint16_t packet_id, const google::protobuf::Message* message = nullptr);

public:
	template <class Insert = std::back_insert_iterator<enet_connection_cb_container>>
	void AddConnectCallback(enet_connection_cb callback) { Insert where(m_connect_cb); *where = callback; }

	template <class Insert = std::back_insert_iterator<enet_connection_cb_container>>
	void AddDisconnectCallback(enet_connection_cb callback) { Insert where(m_disconnect_cb); *where = callback; }

	template <class Insert = std::back_insert_iterator<enet_receive_cb_container>>
	void AddLoginCallback(enet_receive_cb callback) { Insert where(m_login_cb); *where = callback; }

	template <class Insert = std::back_insert_iterator<enet_receive_cb_container>>
	void AddReceiveCallback(enet_receive_cb callback) { Insert where(m_receive_cb); *where = callback; }

	template <class Insert = std::back_insert_iterator<enet_receive_cb_container>>
	void AddClientReceiveCallback(enet_receive_cb callback) { Insert where(m_client_receive_cb); *where = callback; }

public:
	void ExecuteClientReceiveCallback(const uint16_t player_id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length);

private:
	static bool ms_started;
	server::Server* m_server;

	enet_connection_cb_container m_connect_cb;
	enet_connection_cb_container m_disconnect_cb;
	enet_receive_cb_container m_login_cb;
	enet_receive_cb_container m_receive_cb;
	enet_receive_cb_container m_client_receive_cb;
	std::unique_ptr<_ENetHost, enet_host_deleter> m_host;
	std::map<uint16_t, _ENetPeer*> m_peers;
};

} // end namespace tdrp::network
