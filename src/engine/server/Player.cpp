#include "engine/server/Player.h"

namespace tdrp::server
{

bool Player::SwitchScene(std::shared_ptr<scene::Scene> new_scene)
{
	if (auto current_sceneobject = m_current_sceneobject.lock())
	{
		// If this isn't a global scene object, we lose access to it in the new scene.
		if (!current_sceneobject->IsGlobal())
			m_current_sceneobject.reset();
	}

	m_current_scene = new_scene;

	return true;
}

bool Player::SwitchControlledSceneObject(std::shared_ptr<SceneObject> new_scene_object)
{
	m_current_sceneobject = new_scene_object;
	return true;
}

} // end namespace tdrp::server
