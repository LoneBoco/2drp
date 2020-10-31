#include "engine/script/modules/bind.h"

#include "engine/server/Server.h"

namespace tdrp::script::modules
{

void bind_server(sol::state& lua)
{
	lua.new_usertype<server::Server>("Server", sol::no_constructor,
		"OnPlayerJoin", sol::writeonly_property(&server::Server::SetOnPlayerJoin),
		"OnPlayerLeave", sol::writeonly_property(&server::Server::SetOnPlayerLeave),
		"OnServerTick", sol::writeonly_property(&server::Server::SetOnServerTick),

		"CreateSceneObject", &server::Server::CreateSceneObject,
		"DeleteSceneObject", &server::Server::DeleteSceneObject
	);
}

} // end namespace tdrp::script::modules
