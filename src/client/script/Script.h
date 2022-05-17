#pragma once

#include <sol/sol.hpp>

#include "engine/common.h"

namespace tdrp
{

void bind_client(sol::state& lua);

void bind_globals(sol::state& lua);
void bind_camera(sol::state& lua);
void bind_game(sol::state& lua);
void bind_useable(sol::state& lua);

} // end namespace tdrp
