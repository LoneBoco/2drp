#pragma once

#include "engine/common.h"
#include "engine/network/PacketsServer.h"

// Forward declaration.
namespace tdrp { class Game; }

namespace tdrp::handlers
{

void network_connect(Game& game, const uint16_t peer_id);
void network_disconnect(Game& game, const uint16_t peer_id);
void network_receive(Game& game, const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length);

/////////////////////////////

template <class T>
T construct(const uint8_t* const packet_data, const size_t packet_length);

/////////////////////////////

void handle(Game& game, packet::SError& packet);

} // end namespace tdrp::handlers
