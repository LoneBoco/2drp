#pragma once

#include <sol/sol.hpp>

namespace tdrp::script::modules
{

void bind_player(sol::state& lua);
void bind_scene(sol::state& lua);
void bind_sceneobject(sol::state& lua);
void bind_server(sol::state& lua);
void bind_vector(sol::state& lua);

} // end namespace tdrp::script::modules
