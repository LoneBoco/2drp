#include "engine/script/modules/bind.h"

#include "engine/server/Player.h"

namespace tdrp::script::modules
{

void bind_player(sol::state& lua)
{
	lua.new_usertype<server::Player>("Player", sol::no_constructor,
		// "SwitchScene", &server::Player::SwitchScene,
		// "SwitchControlledSceneObject", &server::Player::SwitchControlledSceneObject,

		"GetCurrentScene", &server::Player::LuaGetCurrentScene,
		"GetCurrentControlledSceneObject", &server::Player::LuaGetCurrentControlledSceneObject,

		"GetPlayerId", &server::Player::GetPlayerId
	);
}

} // end namespace tdrp::script::modules
