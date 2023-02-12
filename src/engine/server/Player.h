#pragma once

#include <set>

#include "engine/common.h"

#include "engine/scene/Scene.h"
#include "engine/scene/SceneObject.h"

#include "engine/script/Function.h"

#include "engine/server/Account.h"


namespace tdrp::item
{
	class ItemDefinition;
}


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

	std::weak_ptr<scene::Scene> GetCurrentScene() const;
	std::weak_ptr<SceneObject> GetCurrentControlledSceneObject() const;

	void AddKnownItemDefinition(ItemID baseId);
	bool KnowsItemDefinition(ItemID baseId) const;

	bool HasItem(ItemID id) const;
	bool HasItem(const std::string& name) const;
	item::ItemInstancePtr GetItem(ItemID id) const;
	std::set<item::ItemInstancePtr> GetItems(const item::ItemDefinition* const item_definition) const;
	std::set<item::ItemInstancePtr> GetItems(const std::string& name) const;
	std::set<std::string> GetAllItemTags() const;

	uint16_t GetPlayerId() const;

public:
	std::set<SceneObjectID> FollowedSceneObjects;

public:
	server::Account Account;

protected:
	server::Server* m_server = nullptr;
	uint16_t m_player_id;
	std::weak_ptr<scene::Scene> m_current_scene;
	std::weak_ptr<SceneObject> m_current_sceneobject;
	std::unordered_set<ItemID> m_known_item_definitions;
};

using PlayerPtr = std::shared_ptr<Player>;

/////////////////////////////

inline std::weak_ptr<scene::Scene> Player::GetCurrentScene() const
{
	return m_current_scene;
}

inline std::weak_ptr<SceneObject> Player::GetCurrentControlledSceneObject() const
{
	return m_current_sceneobject;
}

inline void Player::AddKnownItemDefinition(ItemID baseId)
{
	m_known_item_definitions.insert(baseId);
}

inline bool Player::KnowsItemDefinition(ItemID baseId) const
{
	return m_known_item_definitions.contains(baseId);
}

inline uint16_t Player::GetPlayerId() const
{
	return m_player_id;
}

} // end namespace tdrp::server
