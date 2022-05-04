#pragma once

#include "engine/common.h"

// Forward declaration.
namespace tdrp { class Game; }

namespace tdrp::handlers
{

void network_receive_client(Game& game, const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length);

} // end namespace tdrp::handlers
