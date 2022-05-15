#pragma once

#include <set>

#include "engine/common.h"

#include "engine/scene/Scene.h"
#include "engine/scene/SceneObject.h"

#include "engine/script/Function.h"

#include "engine/server/Account.h"


namespace tdrp::server
{

class Server;

class Player
{
	SCRIPT_SETUP;
	SCRIPT_FUNCTION(OnSwitchedControl);

public:
	Player() = delete;
	Player(uint16_t id) : m_player_id(id), Account(this) {}
	~Player() = default;

	Player(const Player& other) = delete;
	Player(Player&& other) = delete;
	Player& operator=(const Player& other) = delete;
	Player& operator=(Player&& other) = delete;

	void BindServer(server::Server* server);
	server::Server* GetServer() const { return m_server; }

	std::weak_ptr<scene::Scene> SwitchScene(std::shared_ptr<scene::Scene>& new_scene);
	std::weak_ptr<SceneObject> SwitchControlledSceneObject(std::shared_ptr<SceneObject>& new_scene_object);

	std::weak_ptr<scene::Scene> GetCurrentScene();
	std::weak_ptr<SceneObject> GetCurrentControlledSceneObject();

	std::shared_ptr<scene::Scene> LuaGetCurrentScene();
	std::shared_ptr<SceneObject> LuaGetCurrentControlledSceneObject();

	uint16_t GetPlayerId() const;

public:
	std::set<uint32_t> FollowedSceneObjects;

public:
	server::Account Account;

protected:
	server::Server* m_server = nullptr;
	uint16_t m_player_id;
	std::weak_ptr<scene::Scene> m_current_scene;
	std::weak_ptr<SceneObject> m_current_sceneobject;
};

using PlayerPtr = std::shared_ptr<Player>;

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
