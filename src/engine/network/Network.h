#pragma once

#include <enet/enet.h>
#include <future>

#include "engine/common.h"
#include "engine/package/Package.h"

namespace tdrp::network
{

struct enet_host_deleter
{
	void operator()(ENetHost* host) const { enet_host_destroy(host); }
};

typedef std::function<void(const uint16_t, ENetEvent&)> enet_cb;

enum class Channel
{
	UNRELIABLE = 0,
	RELIABLE,

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

	bool Initialize(const size_t peers = 1, const uint16_t port = 0);

	void Update();

	void SetConnectCallback(enet_cb callback)
	{
		m_connect_cb = callback;
	}

	void SetDisconnectCallback(enet_cb callback)
	{
		m_disconnect_cb = callback;
	}

	void SetReceiveCallback(enet_cb callback)
	{
		m_receive_cb = callback;
	}

public:
	std::future<bool> Connect(const std::string& hostname, const uint16_t port);
	void Disconnect();

private:
	enet_cb m_connect_cb;
	enet_cb m_disconnect_cb;
	enet_cb m_receive_cb;
	std::unique_ptr<ENetHost, enet_host_deleter> m_host;
	std::vector<ENetPeer*> m_peers;
};

} // end namespace tdrp::network
