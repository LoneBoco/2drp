#pragma once

#include "engine/common.h"

// Forward declaration.
namespace tdrp::server { class Server; class Player; }

namespace tdrp::network::handlers
{

bool network_receive(tdrp::server::Server* server, const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length);

} // end namespace tdrp::network::handlers
