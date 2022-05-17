#include "engine/script/modules/bind.h"

#include "engine/server/Server.h"
#include "engine/scene/Scene.h"

namespace tdrp::script::modules
{

void bind_server(sol::state& lua)
{
	lua.new_usertype<server::Server>("Server", sol::no_constructor,
		"Player", sol::readonly_property(&server::Server::GetPlayer),

		"OnStarted", sol::writeonly_property(&server::Server::SetOnStarted),
		"OnPlayerJoin", sol::writeonly_property(&server::Server::SetOnPlayerJoin),
		"OnPlayerLeave", sol::writeonly_property(&server::Server::SetOnPlayerLeave),
		"OnServerTick", sol::writeonly_property(&server::Server::SetOnServerTick),
		"OnServerEvent", sol::writeonly_property(&server::Server::SetOnServerEvent),
		"OnEvent", sol::writeonly_property(&server::Server::SetOnEvent),

		"GetScene", &server::Server::GetScene,
		"GetDefaultScene", [](server::Server& server) -> std::shared_ptr<scene::Scene> { return server.GetScene(server.GetPackage()->GetStartingScene()); },

		"GiveClientScript", &server::Server::AddPlayerClientScript,
		"RemoveClientScript", &server::Server::RemovePlayerClientScript,

		"CreateSceneObject", &server::Server::CreateSceneObject,
		"DeleteSceneObject", sol::resolve<bool(uint32_t)>(&server::Server::DeleteSceneObject),
		"DeleteSceneObject", sol::resolve<bool(std::shared_ptr<SceneObject>)>(&server::Server::DeleteSceneObject),
		"DeletePlayerOwnedSceneObjects", &server::Server::DeletePlayerOwnedSceneObjects,

		"GetSceneObject", &server::Server::GetSceneObjectById,

		"SwitchPlayerScene", &server::Server::SwitchPlayerScene,
		"SwitchSceneObjectOwnership", &server::Server::SwitchSceneObjectOwnership,

		"SendEvent", &server::Server::SendEvent
	);
}

} // end namespace tdrp::script::modules
