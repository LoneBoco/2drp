#pragma once

#include "engine/common.h"

#include "engine/scene/Scene.h"
#include "engine/scene/SceneObject.h"

namespace tdrp::server
{

class Player
{
public:
	Player() = delete;
	Player(uint16_t id) : m_player_id(id) {}
	~Player() = default;

	Player(const Player& other) = delete;
	Player(Player&& other) = delete;
	Player& operator=(const Player& other) = delete;
	Player& operator=(Player&& other) = delete;

	bool SwitchScene(std::shared_ptr<scene::Scene> new_scene);
	bool SwitchControlledSceneObject(std::shared_ptr<SceneObject> new_scene_object);

	std::weak_ptr<scene::Scene> GetCurrentScene();
	std::weak_ptr<SceneObject> GetCurrentControlledSceneObject();

	std::shared_ptr<scene::Scene> LuaGetCurrentScene();
	std::shared_ptr<SceneObject> LuaGetCurrentControlledSceneObject();

	uint16_t GetPlayerId() const;

protected:
	uint16_t m_player_id;
	std::weak_ptr<scene::Scene> m_current_scene;
	std::weak_ptr<SceneObject> m_current_sceneobject;
};

/////////////////////////////

inline std::weak_ptr<scene::Scene> Player::GetCurrentScene()
{
	return m_current_scene;
}

inline std::weak_ptr<SceneObject> Player::GetCurrentControlledSceneObject()
{
	return m_current_sceneobject;
}

inline std::shared_ptr<scene::Scene> Player::LuaGetCurrentScene()
{
	return m_current_scene.lock();
}

inline std::shared_ptr<SceneObject> Player::LuaGetCurrentControlledSceneObject()
{
	return m_current_sceneobject.lock();
}

inline uint16_t Player::GetPlayerId() const
{
	return m_player_id;
}

} // end namespace tdrp::server
