#include "engine/script/modules/bind.h"

#include "engine/scene/SceneObject.h"

namespace tdrp::script::modules
{

void bind_sceneobject(sol::state& lua)
{
	lua.new_usertype<SceneObject>("SceneObject", sol::no_constructor,
		"Position", sol::property(&SceneObject::GetPosition, &SceneObject::SetPosition),
		"Scale", sol::property(&SceneObject::GetScale, &SceneObject::SetScale),
		"Rotation", sol::property(&SceneObject::GetRotation, &SceneObject::SetRotation),

		"OnCreated", sol::writeonly_property(&SceneObject::SetOnCreated),
		"OnUpdate", sol::writeonly_property(&SceneObject::SetOnUpdate),
		"OnEvent", sol::writeonly_property(&SceneObject::SetOnEvent)
	);
}

} // end namespace tdrp::script::modules
