#include "engine/script/modules/bind.h"

#include "engine/server/Account.h"
#include "engine/server/Player.h"


namespace sol
{
	template <>
	struct is_container<tdrp::ObjectAttributes> : std::false_type {};

	template <>
	struct is_container<tdrp::ObjectProperties> : std::false_type {};
}

namespace tdrp::script::modules
{

namespace functions
{

	scene::ScenePtr get_current_scene(const server::Player& player)
	{
		return player.GetCurrentScene().lock();
	}

	SceneObjectPtr get_current_controlled_scene_object(const server::Player& player)
	{
		return player.GetCurrentControlledSceneObject().lock();
	}

} // end namespace functions


void bind_player(sol::state& lua)
{
	lua.new_usertype<server::Account>("Account", sol::no_constructor,
		"Flags", &server::Account::Flags
	);

	lua.new_usertype<server::Player>("Player", sol::no_constructor,
		//"SwitchScene", &server::Player::SwitchScene,
		"SwitchControlledSceneObject", &server::Player::SwitchControlledSceneObject,
		"GetCurrentScene", &functions::get_current_scene,
		"GetCurrentControlledSceneObject", &functions::get_current_controlled_scene_object,
		"GetPlayerId", &server::Player::GetPlayerId,

		"Account", &server::Player::Account,

		sol::meta_function::equal_to, [](const server::Player& self, const server::Player& other) { return self.GetPlayerId() == other.GetPlayerId(); },

		"OnSwitchedControl", sol::writeonly_property(&server::Player::SetOnSwitchedControl)
	);
}

} // end namespace tdrp::script::modules
