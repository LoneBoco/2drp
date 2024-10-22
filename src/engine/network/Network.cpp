#include <iostream>

#include "engine/network/Network.h"
#include "engine/network/Packet.h"
#include "engine/network/PacketHandler.h"

#include "engine/server/Server.h"

#include <enet/enet.h>

namespace tdrp::network
{

static constexpr uint32_t _determine_flags(const Channel channel)
{
	uint32_t flags = 0;
	switch (channel)
	{
		case Channel::RELIABLE:
		case Channel::FILE:
			flags = ENET_PACKET_FLAG_RELIABLE;
			break;
		case Channel::UNRELIABLE:
			flags = ENET_PACKET_FLAG_UNSEQUENCED;
			break;
	}
	return flags;
}

static std::vector<uint8_t> _serializeMessageToVector(const google::protobuf::Message& message)
{
	size_t message_size = message.ByteSizeLong();

	std::vector<uint8_t> result(message_size);
	message.SerializeToArray(result.data(), static_cast<int>(message_size));

	return result;
}

inline static bool _toSelf(server::Server* server, const uint16_t player_id)
{
	if (!server)
		return false;

	if (server->IsSinglePlayer())
		return true;

	if (server->GetServerType() == server::ServerType::HOST && player_id == network::HOSTID)
		return true;

	return false;
}

template <class T, typename ...Args>
inline void _executeCallbacks(T queue, Args&&... args)
{
	for (auto& cb : queue)
	{
		cb(std::forward<Args>(args)...);
	}
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
		ENetAddress address{};
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
		ENetAddress address{};
		address.port = port;
		if (enet_address_set_host(&address, hostname.c_str()) != 0)
		{
			log::PrintLine("!! Network connect - Failed to determine host address.");
			return false;
		}

		ENetPeer* peer = enet_host_connect(m_host.get(), &address, static_cast<size_t>(Channel::COUNT), 0);
		if (peer == nullptr)
		{
			log::PrintLine("!! Network connect - Failed to connect to host.");
			return false;
		}

		ENetEvent event;
		if (enet_host_service(m_host.get(), &event, 5000) > 0)
		{
			if (event.type == ENET_EVENT_TYPE_CONNECT)
			{
				log::PrintLine(":: Network connect - Established link to host.");
				m_peers.clear();
				m_peers[0] = peer;
				return true;
			}
		}

		log::PrintLine("!! Network connect - Failure to establish link to host.");
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

void Network::DisconnectPeer(const uint16_t player_id)
{
	if (m_peers.size() == 0)
		return;

	enet_peer_disconnect_later(m_peers.at(player_id), 0);
}

/////////////////////////////

void Network::Update()
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
		// Add 1 to get the proper player id.
		uint16_t id = event.peer->outgoingPeerID + 1;
		if (m_server && m_server->IsHost())
			id = event.peer->incomingPeerID + 1;

		// Callbacks.
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				_executeCallbacks(m_connect_cb, id);
				m_peers[id] = event.peer;
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				m_peers.erase(id);
				_executeCallbacks(m_disconnect_cb, id);
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				if (event.packet != nullptr && event.packet->dataLength >= 2)
				{
					// The first two bytes make up the packet id.
					uint16_t packet_id = static_cast<uint16_t>((event.packet->data[0] & 0xFF) | (event.packet->data[1] << 8));
					// log::PrintLine("-- Packet {}", packet_id);

					// If login packet, call the login callback.
					// Otherwise, if we are the host, attempt to handle client packets.
					// Finally, if not handled, pass it to the normal receive callback.
					if (packet_id == PACKETID(Packets::LOGIN) && !m_login_cb.empty())
						_executeCallbacks(m_login_cb, id, packet_id, event.packet->data + 2, event.packet->dataLength - 2);
					else
					{
						_executeCallbacks(m_receive_cb, id, packet_id, event.packet->data + 2, event.packet->dataLength - 2);
						ExecuteClientReceiveCallback(id, packet_id, event.packet->data + 2, event.packet->dataLength - 2);
					}
				}

				enet_packet_destroy(event.packet);
				break;
		}
	}
}

/////////////////////////////

void Network::Send(const uint16_t player_id, const uint16_t packet_id, const Channel channel)
{
	// Check for a network bypass.
	if (_toSelf(m_server, player_id))
	{
		ExecuteClientReceiveCallback(0, packet_id, nullptr, 0);
		return;
	}

	auto peer = m_peers.find(player_id);
	if (peer == m_peers.end() || peer->second == nullptr)
		return;

	ENetPacket* packet = construct_packet(channel, packet_id);
	if (packet == nullptr) return;

	auto success = enet_peer_send(peer->second, static_cast<uint8_t>(channel), packet);
	if (success != 0)
	{
		enet_packet_destroy(packet);
		log::PrintLine("** ERROR: Failure to send packet {} to player {}.", packet_id, player_id);
	}
}

void Network::Send(const uint16_t player_id, const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message)
{
	// Check for a network bypass.
	if (_toSelf(m_server, player_id))
	{
		auto data = _serializeMessageToVector(message);
		ExecuteClientReceiveCallback(0, packet_id, data.data(), data.size());
		return;
	}

	auto peer = m_peers.find(player_id);
	if (peer == m_peers.end() || peer->second == nullptr)
		return;

	ENetPacket* packet = construct_packet(channel, packet_id, &message);
	if (packet == nullptr) return;

	auto success = enet_peer_send(peer->second, static_cast<uint8_t>(channel), packet);
	if (success != 0)
	{
		enet_packet_destroy(packet);
		log::PrintLine("** ERROR: Failure to send packet {} to player {}.", packet_id, player_id);
	}
}

void Network::Broadcast(const uint16_t packet_id, const Channel channel)
{
	// Check for a network bypass.
	// Broadcast shouldn't need this.
	/*
	if (_toSelf(m_server, packet_id network::HOSTID))
	{
		ExecuteClientReceiveCallback(0, packet_id, nullptr, 0);
	}
	*/

	ENetPacket* packet = construct_packet(channel, packet_id);
	if (packet == nullptr) return;

	enet_host_broadcast(m_host.get(), static_cast<uint8_t>(channel), packet);
}

void Network::Broadcast(const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message)
{
	ENetPacket* packet = construct_packet(channel, packet_id, &message);
	if (packet == nullptr) return;

	enet_host_broadcast(m_host.get(), static_cast<uint8_t>(channel), packet);
}

std::vector<server::PlayerPtr> Network::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, uint16_t packet_id, const Channel channel, const std::set<server::PlayerPtr>& exclude)
{
	// Network bypass is in the SendToScene function that iterates through the scene players.

	// Guest should just send to the server.  They won't have other peers.
	if (m_server && m_server->IsGuest())
	{
		assert(false);
		//Send(network::HOSTID, packet_id, channel);
		return {};
	}

	return SendToScene(scene, location, packet_id, channel, construct_packet(channel, packet_id), exclude);
}

std::vector<server::PlayerPtr> Network::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message, const std::set<server::PlayerPtr>& exclude)
{
	// Network bypass is in the SendToScene function that iterates through the scene players.

	// Guest should just send to the server.  They won't have other peers.
	if (m_server && m_server->IsGuest())
	{
		assert(false);
		//Send(network::HOSTID, packet_id, channel, message);
		return {};
	}

	return SendToScene(scene, location, packet_id, channel, construct_packet(channel, packet_id, &message), exclude);
}

int Network::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, const std::set<server::PlayerPtr>& exclude)
{
	// Network bypass is in the SendToScene function that iterates through the scene players.

	return BroadcastToScene(scene, packet_id, channel, construct_packet(channel, packet_id), exclude);
}

int Network::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, const google::protobuf::Message& message, const std::set<server::PlayerPtr>& exclude)
{
	// Network bypass is in the SendToScene function that iterates through the scene players.

	return BroadcastToScene(scene, packet_id, channel, construct_packet(channel, packet_id, &message), exclude);
}

/////////////////////////////

std::vector<server::PlayerPtr> Network::SendToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const Vector2df location, const uint16_t packet_id, const Channel channel, _ENetPacket* packet, const std::set<server::PlayerPtr>& exclude)
{
	std::vector<server::PlayerPtr> result;
	
	if (packet == nullptr)
		return result;

	auto original_ref_count = packet->referenceCount;

	for (auto& [player_id, player] : m_server->m_player_list)
	{
		if (exclude.contains(player))
			continue;

		// Assemble a list of owned scene objects.
		std::set<SceneObjectPtr> owned_objects;
		std::for_each(std::begin(player->FollowedSceneObjects), std::end(player->FollowedSceneObjects),
			[&, this](const auto id)
			{
				auto so = m_server->GetSceneObjectById(id);
				if (!so) return;

				if (so->GetOwningPlayer().lock() == player)
					owned_objects.insert(so);
			}
		);

		if (const auto player_scene = player->GetCurrentScene().lock())
		{
			if (player_scene.get() == scene.get())
			{
				// Check of any of the owned scene objects are in range.
				bool in_range = false;

				(void)std::any_of(std::begin(owned_objects), std::end(owned_objects),
					[&, this](const auto& so) -> bool
					{
						auto distance = Vector2df::Distance(location, so->GetPosition());
						if (distance <= scene->GetTransmissionDistance())
						{
							in_range = true;
							return true;
						}
						return false;
					}
				);

				if (in_range)
				{
					result.push_back(player);

					// If this is ourself, skip the network send.
					if (m_server->GetPlayer()->GetPlayerId() == player_id)
					{
						ExecuteClientReceiveCallback(player_id, packet_id, packet->data + 2, packet->dataLength - 2);
					}
					// Send the packet.
					else
					{
						auto peer = m_peers.find(player_id);
						if (peer == std::end(m_peers) || peer->second == nullptr)
							continue;

						auto success = enet_peer_send(peer->second, static_cast<uint8_t>(channel), packet);
						if (success != 0)
						{
							log::PrintLine("** ERROR: Failure to send packet {} to player {} in scene \"{}\".", packet_id, player_id, scene->GetName());
							continue;
						}
					}
				}
			}
		}
	}

	// If the packet was not accepted by enet, destroy it.
	if (packet->referenceCount == original_ref_count)
		enet_packet_destroy(packet);

	return result;
}

int Network::BroadcastToScene(const std::shared_ptr<tdrp::scene::Scene> scene, const uint16_t packet_id, const Channel channel, _ENetPacket* packet, const std::set<server::PlayerPtr>& exclude)
{
	if (packet == nullptr) return 0;
	int count = 0;
	auto original_ref_count = packet->referenceCount;

	for (auto& [player_id, player] : m_server->m_player_list)
	{
		if (exclude.contains(player))
			continue;

		if (auto player_scene = player->GetCurrentScene().lock())
		{
			if (player_scene.get() == scene.get())
			{
				++count;

				// If this is ourself, skip the network send.
				if (m_server->GetPlayer()->GetPlayerId() == player_id)
				{
					ExecuteClientReceiveCallback(player_id, packet_id, packet->data + 2, packet->dataLength - 2);
				}
				else
				// Send the packet.
				{
					auto peer = m_peers.find(player_id);
					if (peer == std::end(m_peers) || peer->second == nullptr)
						continue;

					auto success = enet_peer_send(peer->second, static_cast<uint8_t>(channel), packet);
					if (success != 0)
					{
						log::PrintLine("** ERROR: Failure to send packet {} to player {} in scene \"{}\".", packet_id, player_id, scene->GetName());
						continue;
					}
				}
			}
		}
	}

	// If the packet was not accepted by enet, destroy it.
	if (packet->referenceCount == original_ref_count)
		enet_packet_destroy(packet);

	return count;
}

/////////////////////////////

_ENetPacket* Network::construct_packet(const Channel channel, const uint16_t packet_id, const google::protobuf::Message* message)
{
	uint32_t flags = _determine_flags(channel);
	size_t message_size = (message == nullptr) ? 0 : message->ByteSizeLong();
	ENetPacket* packet = enet_packet_create(nullptr, 2 + message_size, flags);
	if (packet == nullptr)
		return nullptr;

	packet->data[0] = static_cast<uint8_t>(packet_id & 0xFF);
	packet->data[1] = static_cast<uint8_t>((packet_id >> 8) & 0xFF);
	if (message != nullptr)
		message->SerializeToArray(packet->data + 2, static_cast<int>(message_size));

	return packet;
}

void Network::ExecuteClientReceiveCallback(const uint16_t player_id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	_executeCallbacks(m_client_receive_cb, player_id, packet_id, packet_data, packet_length);
}

} // end namespace tdrp::network
