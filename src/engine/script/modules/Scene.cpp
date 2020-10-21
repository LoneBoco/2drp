#include "engine/script/modules/bind.h"

#include "engine/scene/Scene.h"

namespace tdrp::script::modules
{

void bind_scene(sol::state& lua)
{
	lua.new_usertype<scene::Scene>("Scene", sol::no_constructor,
		"GetName", &scene::Scene::GetName,

		"AddObject", &scene::Scene::AddObject,
		"RemoveObject", &scene::Scene::RemoveObject,

		"FindObjectById", sol::resolve<std::shared_ptr<SceneObject>(uint32_t) const>(&scene::Scene::FindObject),
		"FindObjectByName", sol::resolve<std::shared_ptr<SceneObject>(const std::string&) const>(&scene::Scene::FindObject),

		"FindObjectsInRangeOf", &scene::Scene::FindObjectsInRangeOf,

		"GetTransmissionDistance", &scene::Scene::GetTransmissionDistance
	);
}

} // end namespace tdrp::script::modules
