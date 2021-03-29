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

bool load_model(SceneObject& self, const std::string& model);
bool load_model(SceneObject& self, const std::string& model, const std::string& entity);

std::string get_animation(SceneObject& self);
void set_animation(SceneObject& self, const std::string& animation);


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

		"Animation", sol::property(&get_animation, &set_animation),

		"LoadModel", sol::overload(
			sol::resolve<bool(SceneObject&, const std::string&)>(&load_model),
			sol::resolve<bool(SceneObject&, const std::string&, const std::string&)>(load_model)
		),

		"Attributes", &SceneObject::Attributes,
		"Properties", &SceneObject::Properties,

		"OnCreated", sol::writeonly_property(&SceneObject::SetOnCreated),
		"OnUpdate", sol::writeonly_property(&SceneObject::SetOnUpdate),
		"OnEvent", sol::writeonly_property(&SceneObject::SetOnEvent),
		"OnPlayerGainedControl", sol::writeonly_property(&SceneObject::SetOnPlayerGainedControl),
		"OnCollision", sol::writeonly_property(&SceneObject::SetOnCollision)
	);
}


bool load_model(SceneObject& self, const std::string& model)
{
	if (self.GetType() != SceneObjectType::ANIMATED)
		return false;

	auto animated_so = dynamic_cast<AnimatedSceneObject*>(&self);
	if (!animated_so)
		return false;

	animated_so->SetModel(model);
	return true;
}

bool load_model(SceneObject& self, const std::string& model, const std::string& entity)
{
	if (self.GetType() != SceneObjectType::ANIMATED)
		return false;

	auto animated_so = dynamic_cast<AnimatedSceneObject*>(&self);
	if (!animated_so)
		return false;

	animated_so->SetModel(model, entity);
	return true;
}

std::string get_animation(SceneObject& self)
{
	if (self.GetType() != SceneObjectType::ANIMATED)
		return {};

	auto animated_so = dynamic_cast<AnimatedSceneObject*>(&self);
	if (!animated_so)
		return {};

	return animated_so->GetAnimation();
}

void set_animation(SceneObject& self, const std::string& animation)
{
	if (self.GetType() != SceneObjectType::ANIMATED)
		return;

	auto animated_so = dynamic_cast<AnimatedSceneObject*>(&self);
	if (!animated_so)
		return;

	animated_so->SetAnimation(animation);
}

} // end namespace tdrp::script::modules
