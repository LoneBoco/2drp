#include "engine/script/modules/bind.h"

#include "engine/scene/SceneObject.h"

namespace tdrp::script::modules
{

void bind_sceneobject(sol::state& lua)
{
	lua.new_enum("SceneObjectType",
		"DEFAULT", SceneObjectType::DEFAULT,
		"STATIC", SceneObjectType::STATIC,
		"ANIMATED", SceneObjectType::ANIMATED,
		"TILEMAP", SceneObjectType::TILEMAP,
		"TMX", SceneObjectType::TMX
	);

	lua.new_usertype<SceneObject>("SceneObject", sol::no_constructor,
		"ID", &SceneObject::ID,

		"Position", sol::property(&SceneObject::GetPosition, &SceneObject::SetPosition),
		"Scale", sol::property(&SceneObject::GetScale, &SceneObject::SetScale),
		"Rotation", sol::property(&SceneObject::GetRotation, &SceneObject::SetRotation),
		"Image", sol::property(&SceneObject::GetImage, &SceneObject::SetImage),
		"Layer", sol::property(&SceneObject::GetDepth, &SceneObject::SetDepth),

		"OnCreated", sol::writeonly_property(&SceneObject::SetOnCreated),
		"OnUpdate", sol::writeonly_property(&SceneObject::SetOnUpdate),
		"OnEvent", sol::writeonly_property(&SceneObject::SetOnEvent),
		"OnPlayerFollowed", sol::writeonly_property(&SceneObject::SetOnPlayerFollowed),

		"vars", &SceneObject::LocalData
	);
}

} // end namespace tdrp::script::modules
