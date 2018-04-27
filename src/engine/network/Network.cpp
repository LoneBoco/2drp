#include "engine/network/Network.h"

namespace tdrp::network
{

Network::Network()
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
		address.host = ENET_HOST_ANY;
		address.port = port;

		m_host = std::unique_ptr<ENetHost, enet_host_deleter>(enet_host_create(&address, peers, channels, bandwidth_downstream, bandwidth_upstream));

		m_peers.reserve(peers);
	}
	
	if (m_host == nullptr)
		return false;

	return true;
}

std::future<bool> Network::Connect(const std::string& hostname, const uint16_t port)
{
	auto f = std::async(std::launch::async, [&] () -> bool {
		ENetAddress address;
		if (enet_address_set_host(&address, hostname.c_str()) != 0)
			return false;

		address.port = port;

		ENetPeer* peer = enet_host_connect(m_host.get(), &address, static_cast<size_t>(Channel::COUNT), 0);
		if (peer == nullptr)
			return false;

		ENetEvent event;
		if (enet_host_service(m_host.get(), &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			m_peers.clear();
			m_peers.push_back(peer);
			return true;
		}

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

void Network::Update()
{
	if (!m_host) return;
	if (m_host->connectedPeers == 0) return;

	ENetEvent event;
	while (enet_host_service(m_host.get(), &event, 0) > 0)
	{
		// Ignore NONE events.
		if (event.type == ENET_EVENT_TYPE_NONE)
			continue;

		uint32_t id = 0;
		auto peer = std::find(m_peers.begin(), m_peers.end(), event.peer);
		if (peer != m_peers.end())
			id = static_cast<uint32_t>(std::distance(m_peers.begin(), peer));

		// New peer with unused id.
		if (event.type == ENET_EVENT_TYPE_CONNECT && peer == m_peers.end())
		{
			m_peers.push_back(event.peer);
			id = m_peers.size() - 1;
		}

		// Sanity check.
		if (peer == m_peers.end())
			continue;

		// Callbacks.
		switch (event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				if (m_connect_cb)
					m_connect_cb(id, event);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				if (m_disconnect_cb)
					m_disconnect_cb(id, event);

				event.peer->data = nullptr;
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				if (m_receive_cb && event.packet != nullptr && event.packet->dataLength != 0)
					m_receive_cb(id, event);

				enet_packet_destroy(event.packet);
				break;
		}
	}
}

} // end namespace tdrp::network
