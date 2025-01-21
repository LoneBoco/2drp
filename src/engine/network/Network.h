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

struct enet_host_deleter
{
	void operator()(_ENetHost* host) const;
};

//! Peer ID
typedef std::function<void(const uint32_t)> enet_connection_cb;

//! Player ID
typedef std::function<void(const PlayerID)> enet_disconnection_cb;

//! Player ID, Packet ID, Packet Data, Packet Length
typedef std::function<void(const PlayerID, const uint16_t, const uint8_t* const, const size_t) > enet_receive_cb;

using enet_connection_cb_container = std::deque<enet_connection_cb>;
using enet_disconnection_cb_container = std::deque<enet_disconnection_cb>;
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
	void DisconnectPeer(const PlayerID player_id);

public:
	void Update();

public:
	void Send(const uint16_t packet_id, const Channel channel);
	void Send(const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message);
	void Send(const PlayerID player_id, const uint16_t packet_id, const Channel channel);
	void Send(const PlayerID player_id, const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message);
	void Broadcast(const uint16_t packet_id, const Channel channel);
	void Broadcast(const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message);
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, uint16_t packet_id, const Channel channel, const std::set<server::PlayerPtr>& exclude = {});
	std::vector<server::PlayerPtr> SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message, const std::set<server::PlayerPtr>& exclude = {});
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, const std::set<server::PlayerPtr>& exclude = {});
	int BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message, const std::set<server::PlayerPtr>& exclude = {});

private:
	std::vector<server::PlayerPtr> sendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, _ENetPacket* packet, const std::set<server::PlayerPtr>& exclude = {});
	int broadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, _ENetPacket* packet, const std::set<server::PlayerPtr>& exclude = {});
	_ENetPacket* construct_packet(const Channel channel, const uint16_t packet_id, const google::protobuf::Message* message = nullptr);

public:
	template <class Insert = std::back_insert_iterator<enet_connection_cb_container>>
	void AddConnectCallback(enet_connection_cb callback) { Insert where(m_connect_cb); *where = callback; }

	template <class Insert = std::back_insert_iterator<enet_disconnection_cb_container>>
	void AddDisconnectCallback(enet_disconnection_cb callback) { Insert where(m_disconnect_cb); *where = callback; }

	template <class Insert = std::back_insert_iterator<enet_receive_cb_container>>
	void AddLoginCallback(enet_receive_cb callback) { Insert where(m_login_cb); *where = callback; }

	template <class Insert = std::back_insert_iterator<enet_receive_cb_container>>
	void AddReceiveCallback(enet_receive_cb callback) { Insert where(m_receive_cb); *where = callback; }

	template <class Insert = std::back_insert_iterator<enet_receive_cb_container>>
	void AddClientReceiveCallback(enet_receive_cb callback) { Insert where(m_client_receive_cb); *where = callback; }

	std::optional<uint32_t> GetPeerIdForPlayer(const PlayerID player_id);

	void MapPlayerToPeer(const PlayerID player_id, const uint32_t peer_id);

public:
	void ExecuteClientReceiveCallback(const PlayerID player_id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length);

private:
	void check_for_error(PlayerID player_id, _ENetPeer* peer);
	std::optional<std::pair<_ENetPeer*, uint32_t>> get_peer_from_player(const PlayerID player_id) const;
	void register_player_and_peer(const PlayerID player_id, _ENetPeer* peer);

private:
	static bool ms_started;
	server::Server* m_server;

	enet_connection_cb_container m_connect_cb;
	enet_disconnection_cb_container m_disconnect_cb;
	enet_receive_cb_container m_login_cb;
	enet_receive_cb_container m_receive_cb;
	enet_receive_cb_container m_client_receive_cb;
	std::unique_ptr<_ENetHost, enet_host_deleter> m_enet;
	std::unordered_map<uint32_t, std::pair<_ENetPeer*, PlayerID>> m_peers;
	std::unordered_map<PlayerID, uint32_t> m_player_id_map;
};

inline std::optional<uint32_t> Network::GetPeerIdForPlayer(const PlayerID player_id)
{
	if (auto iter = m_player_id_map.find(player_id); iter != std::end(m_player_id_map))
		return iter->second;
	return std::nullopt;
}

inline void Network::MapPlayerToPeer(const PlayerID player_id, const uint32_t peer_id)
{
	m_player_id_map[player_id] = peer_id;
	m_peers[peer_id].second = player_id;
}

} // end namespace tdrp::network
