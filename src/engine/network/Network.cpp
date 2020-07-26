#include <iostream>

#include <enet/enet.h>

#include "engine/network/Network.h"
#include "engine/network/Packet.h"
#include "engine/network/ClientPacketHandler.h"

#include "engine/server/Server.h"

namespace tdrp::network
{

constexpr uint32_t _determine_flags(const Channel channel)
{
	uint32_t flags = 0;
	switch (channel)
	{
		case Channel::RELIABLE:
			flags = ENET_PACKET_FLAG_RELIABLE;
			break;
		case Channel::UNRELIABLE:
			flags = ENET_PACKET_FLAG_UNSEQUENCED;
			break;
	}
	return flags;
}

/////////////////////////////

inline void enet_host_deleter::operator()(_ENetHost* host) const
{
	enet_host_destroy(host);
}

/////////////////////////////

bool Network::ms_started = false;
bool Network::Startup()
{
	if (ms_started)
		return true;

	int ret = enet_initialize();
	return ms_started = (ret == 0);
}

void Network::Shutdown()
{
	if (ms_started)
		enet_deinitialize();
	ms_started = false;
}

/////////////////////////////

Network::Network()
	: m_server(nullptr)
{
}

Network::~Network()
{
}

bool Network::Initialize(const size_t peers, const uint16_t port)
{
	const size_t channels = static_cast<size_t>(Channel::COUNT);
	const uint32_t bandwidth_downstream = 0;
	const uint32_t bandwidth_upstream = 0;

	if (port == 0)
		m_host = std::unique_ptr<ENetHost, enet_host_deleter>(enet_host_create(nullptr, peers, channels, bandwidth_downstream, bandwidth_upstream));
	else
	{
		ENetAddress address;
		//enet_address_set_host(&address, "127.0.0.1");
		address.host = ENET_HOST_ANY;
		address.port = port;

		m_host = std::unique_ptr<ENetHost, enet_host_deleter>(enet_host_create(&address, peers, channels, bandwidth_downstream, bandwidth_upstream));
	}
	
	if (m_host == nullptr)
		return false;

	return true;
}

void Network::BindServer(server::Server* server)
{
	m_server = server;
}

/////////////////////////////

std::future<bool> Network::Connect(const std::string& hostname, const uint16_t port)
{
	auto f = std::async(std::launch::async, [=] () -> bool
	{
		ENetAddress address;
		address.port = port;
		if (enet_address_set_host(&address, hostname.c_str()) != 0)
		{
			std::cout << "!! Network connect - Failed to determine host address." << std::endl;
			return false;
		}

		ENetPeer* peer = enet_host_connect(m_host.get(), &address, static_cast<size_t>(Channel::COUNT), 0);
		if (peer == nullptr)
		{
			std::cout << "!! Network connect - Failed to connect to host." << std::endl;
			return false;
		}

		ENetEvent event;
		if (enet_host_service(m_host.get(), &event, 5000) > 0)
		{
			if (event.type == ENET_EVENT_TYPE_CONNECT)
			{
				std::cout << ":: Network connect - Established link to host." << std::endl;
				m_peers.clear();
				m_peers[0] = peer;
				return true;
			}
		}

		std::cout << "!! Network connect - Failure to establish link to host." << std::endl;
		enet_peer_reset(peer);
		return false;
	});

	return f;
}

void Network::Disconnect()
{
	if (m_peers.size() == 0)
		return;

	enet_peer_disconnect(m_peers.at(0), 0);

	// TODO: Flag that we are disconnecting.
	// Don't handle packets when disconnecting.
	// If we aren't disconnecting after a certain time, force with enet_peer_reset.
}

void Network::DisconnectPeer(const uint16_t peer_id)
{
	if (m_peers.size() == 0)
		return;

	enet_peer_disconnect_later(m_peers.at(peer_id), 0);
}

/////////////////////////////

void Network::Update(bool isServer)
{
	if (!m_host) return;
	//if (m_host->connectedPeers == 0) return;

	ENetEvent event;
	while (enet_host_service(m_host.get(), &event, 0) > 0)
	{
		// Ignore NONE events.
		if (event.type == ENET_EVENT_TYPE_NONE)
			continue;

		// Get our peer id.
		uint16_t id = isServer ? event.peer->incomingPeerID : event.peer->outgoingPeerID;

		// Callbacks.
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				if (m_connect_cb)
					m_connect_cb(id);

				m_peers[id] = event.peer;
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				if (m_disconnect_cb)
					m_disconnect_cb(id);

				// Delete our account data tied to this player.
				if (event.peer->data != nullptr)
				{
					auto peeraccount = static_cast<server::Account*>(event.peer->data);
					delete peeraccount;
				}

				event.peer->data = nullptr;
				m_peers.erase(id);
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				if (event.packet != nullptr && event.packet->dataLength >= 2)
				{
					// The first two bytes make up the packet id.
					uint16_t packet_id = static_cast<uint16_t>((event.packet->data[0] & 0xFF) | (event.packet->data[1] << 8));

					// If login packet, call the login callback.
					// Otherwise, if we are the host, attempt to handle client packets.
					// Finally, if not handled, pass it to the normal receive callback.
					if (packet_id == PACKETID(ClientPackets::LOGIN) && m_login_cb)
						m_login_cb(id, packet_id, event.packet->data + 2, event.packet->dataLength - 2);
					else
					{
						bool handled = false;

						// If we are a server, handle incoming client packets.
						if (isServer)
							handled = tdrp::network::handlers::network_receive(std::shared_ptr<server::Server>(m_server), id, packet_id, event.packet->data + 2, event.packet->dataLength - 2);
						
						// If we have a receive callback and we haven't handled our packet yet, do the callback.
						if (!handled && m_receive_cb)
							m_receive_cb(id, packet_id, event.packet->data + 2, event.packet->dataLength - 2);
					}
				}

				enet_packet_destroy(event.packet);
				break;
		}
	}
}

/////////////////////////////

void Network::Send(const uint16_t peer_id, const uint16_t packet_id, const Channel channel)
{
	if (m_peers.find(peer_id) == m_peers.end()) return;

	ENetPacket* packet = construct_packet(channel, packet_id);
	if (packet == nullptr) return;

	enet_peer_send(m_peers[peer_id], static_cast<uint8_t>(channel), packet);
}

void Network::Send(const uint16_t peer_id, const uint16_t packet_id, const Channel channel, google::protobuf::Message& message)
{
	if (m_peers.find(peer_id) == m_peers.end()) return;

	ENetPacket* packet = construct_packet(channel, packet_id, &message);
	if (packet == nullptr) return;

	enet_peer_send(m_peers[peer_id], static_cast<uint8_t>(channel), packet);
}

void Network::Broadcast(const uint16_t packet_id, const Channel channel)
{
	if (m_host == nullptr) return;

	ENetPacket* packet = construct_packet(channel, packet_id);
	if (packet == nullptr) return;

	enet_host_broadcast(m_host.get(), static_cast<uint8_t>(channel), packet);
}

void Network::Broadcast(const uint16_t packet_id, const Channel channel, google::protobuf::Message& message)
{
	if (m_host == nullptr) return;

	ENetPacket* packet = construct_packet(channel, packet_id, &message);
	if (packet == nullptr) return;

	enet_host_broadcast(m_host.get(), static_cast<uint8_t>(channel), packet);
}

int Network::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, uint16_t packet_id, const Channel channel)
{
	if (m_host == nullptr) return 0;
	return SendToScene(scene, location, packet_id, channel, construct_packet(channel, packet_id));
}

int Network::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, google::protobuf::Message& message)
{
	if (m_host == nullptr) return 0;
	return SendToScene(scene, location, packet_id, channel, construct_packet(channel, packet_id, &message));
}

int Network::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel)
{
	if (m_host == nullptr) return 0;
	return BroadcastToScene(scene, packet_id, channel, construct_packet(channel, packet_id));
}

int Network::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, google::protobuf::Message& message)
{
	if (m_host == nullptr) return 0;
	return BroadcastToScene(scene, packet_id, channel, construct_packet(channel, packet_id, &message));
}

/////////////////////////////

int Network::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, _ENetPacket* packet)
{
	if (packet == nullptr) return 0;

	int count = 0;
	for (auto[player_id, player] : m_server->m_player_list)
	{
		if (auto player_scene = player->GetCurrentScene().lock())
		{
			if (*player_scene == *scene)
			{
				if (auto player_object = player->GetCurrentControlledSceneObject().lock())
				{
					auto distance = Vector2df::DistanceSquared(location, player_object->GetPosition());
					if (distance <= scene->GetTransmissionDistance())
					{
						++count;
						enet_peer_send(m_peers[player_id], static_cast<uint8_t>(channel), packet);
					}
				}
			}
		}
	}

	return count;
}

int Network::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, _ENetPacket* packet)
{
	if (packet == nullptr) return 0;

	int count = 0;
	for (auto[player_id, player] : m_server->m_player_list)
	{
		if (auto player_scene = player->GetCurrentScene().lock())
		{
			if (*player_scene == *scene)
			{
				++count;
				enet_peer_send(m_peers[player_id], static_cast<uint8_t>(channel), packet);
			}
		}
	}

	return count;
}

/////////////////////////////

bool Network::BindAccountToPeer(const uint16_t peer_id, std::unique_ptr<server::Account>&& account)
{
	if (m_peers.find(peer_id) == m_peers.end())
		return false;

	auto peeraccount = static_cast<server::Account*>(m_peers[peer_id]->data);
	if (peeraccount != nullptr)
		delete peeraccount;

	m_peers[peer_id]->data = account.release();
	return true;
}

server::Account* Network::GetAccountFromPeer(const uint16_t peer_id)
{
	auto iter = m_peers.find(peer_id);
	if (iter == m_peers.end())
		return nullptr;

	return static_cast<server::Account*>(iter->second->data);
}

/////////////////////////////

_ENetPacket* Network::construct_packet(const Channel channel, const uint16_t packet_id, google::protobuf::Message* message)
{
	uint32_t flags = _determine_flags(channel);
	size_t message_size = (message == nullptr) ? 0 : message->ByteSizeLong();
	ENetPacket* packet = enet_packet_create(nullptr, 2 + message_size, flags);
	if (packet == nullptr)
		return nullptr;

	packet->data[0] = static_cast<uint8_t>(packet_id & 0xFF);
	packet->data[1] = static_cast<uint8_t>((packet_id >> 8) & 0xFF);
	if (message != nullptr)
		message->SerializeToArray(packet->data + 2, message_size);

	return packet;
}

} // end namespace tdrp::network
