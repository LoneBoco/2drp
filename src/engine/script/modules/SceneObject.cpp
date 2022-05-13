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

namespace functions
{
	template <typename T>
	T* convert(SceneObject& so)
	{
		auto test = dynamic_cast<T*>(&so);
		if (test == nullptr)
			return nullptr;

		return test;
	}

	#define CONVERSION(T) static_cast<T* (*)(SceneObject&)>(&functions::convert)

	std::string get_class_name(const SceneObject& self)
	{
		auto c = self.GetClass().lock();
		if (c) return c->GetName();
		else return {};
	}
}

void bind_sceneobject(sol::state& lua)
{
	lua.new_enum("SceneObjectType",
		"DEFAULT", SceneObjectType::DEFAULT,
		"STATIC", SceneObjectType::STATIC,
		"ANIMATED", SceneObjectType::ANIMATED,
		"TILEMAP", SceneObjectType::TILEMAP,
		"TMX", SceneObjectType::TMX,
		"TEXT", SceneObjectType::TEXT
	);

	auto so_as = lua.create_named_table("_SceneObjectAs");
	so_as["TEXT"] = CONVERSION(TextSceneObject);


	lua.new_usertype<SceneObject>("SceneObject", sol::no_constructor,
		"ID", &SceneObject::ID,
		"Type", sol::readonly_property(&SceneObject::GetType),
		"ClassName", sol::readonly_property(&functions::get_class_name),

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
		"Text", sol::property(&SceneObject::GetText, &SceneObject::SetText),

		"AttachTo", &SceneObject::AttachTo,

		"Attributes", &SceneObject::Attributes,
		"Properties", &SceneObject::Properties,

		"Visible", &SceneObject::Visible,
		"NonReplicated", sol::readonly_property(&SceneObject::NonReplicated),

		"OnCreated", sol::writeonly_property(&SceneObject::SetOnCreated),
		"OnUpdate", sol::writeonly_property(&SceneObject::SetOnUpdate),
		"OnEvent", sol::writeonly_property(&SceneObject::SetOnEvent),
		"OnOwnershipChange", sol::writeonly_property(&SceneObject::SetOnOwnershipChange),
		"OnCollision", sol::writeonly_property(&SceneObject::SetOnCollision),
		"OnAnimationEnd", sol::writeonly_property(&SceneObject::SetOnAnimationEnd),

		"As", sol::readonly_property([&lua]() { return lua["_SceneObjectAs"]; })
	);

	lua.new_usertype<TextSceneObject>("TextSceneObject", sol::no_constructor,
		"Font", sol::property(&TextSceneObject::GetFont, &TextSceneObject::SetFont),
		"FontSize", sol::property(&TextSceneObject::GetFontSize, &TextSceneObject::SetFontSize),
		"Centered", sol::property(&TextSceneObject::GetCentered, &TextSceneObject::SetCentered),

		sol::base_classes, sol::bases<SceneObject>()
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
