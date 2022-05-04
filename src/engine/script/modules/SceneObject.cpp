#include "engine/script/modules/bind.h"

#include "engine/scene/SceneObject.h"

namespace sol
{
	template <>
	struct is_container<tdrp::ObjectAttributes> : std::false_type {};

	template <>
	struct is_container<tdrp::ObjectProperties> : std::false_type {};
}

namespace tdrp::script::modules
{

std::string get_class_name(const SceneObject& self)
{
	auto c = self.GetClass().lock();
	if (c) return c->GetName();
	else return {};
}

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
		"Type", sol::readonly_property(&SceneObject::GetType),
		"ClassName", sol::readonly_property(&get_class_name),

		"Position", sol::property(&SceneObject::GetPosition, &SceneObject::SetPosition),
		"Layer", sol::property(&SceneObject::GetDepth, &SceneObject::SetDepth),
		"Scale", sol::property(&SceneObject::GetScale, &SceneObject::SetScale),
		// Velocity
		// Force
		// Torque
		"Rotation", sol::property(&SceneObject::GetRotation, &SceneObject::SetRotation),
		"Direction", sol::property(&SceneObject::GetDirection, &SceneObject::SetDirection),
		"Image", sol::property(&SceneObject::GetImage, &SceneObject::SetImage),
		"Entity", sol::property(&SceneObject::GetEntity, &SceneObject::SetEntity),
		"Animation", sol::property(&SceneObject::GetAnimation, &SceneObject::SetAnimation),

		"Attributes", &SceneObject::Attributes,
		"Properties", &SceneObject::Properties,
		
		"Visible", &SceneObject::Visible,
		"NonReplicated", sol::readonly_property(&SceneObject::NonReplicated),

		"OnCreated", sol::writeonly_property(&SceneObject::SetOnCreated),
		"OnUpdate", sol::writeonly_property(&SceneObject::SetOnUpdate),
		"OnEvent", sol::writeonly_property(&SceneObject::SetOnEvent),
		"OnOwnershipChange", sol::writeonly_property(&SceneObject::SetOnOwnershipChange),
		"OnCollision", sol::writeonly_property(&SceneObject::SetOnCollision),
		"OnAnimationEnd", sol::writeonly_property(&SceneObject::SetOnAnimationEnd)
	);
}

AnimatedSceneObject* asAnimated(SceneObject& self)
{
	if (self.GetType() != SceneObjectType::ANIMATED)
		return nullptr;

	auto animated_so = dynamic_cast<AnimatedSceneObject*>(&self);
	return animated_so;
}

} // end namespace tdrp::script::modules
