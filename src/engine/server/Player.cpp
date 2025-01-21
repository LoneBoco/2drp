#include "engine/server/Player.h"

#include "engine/server/Server.h"
#include "engine/network/Packet.h"
#include "engine/network/packets/SceneObjectControl.pb.h"
#include "engine/network/construct/SceneObject.h"

namespace tdrp::server
{

void Player::BindServer(server::Server* server)
{
	m_server = server;
}

std::weak_ptr<scene::Scene> Player::SwitchScene(std::shared_ptr<scene::Scene>& new_scene)
{
	auto old_scene = m_current_scene;

	if (auto so = m_current_sceneobject.lock(); so != nullptr)
	{
		// If this isn't a global scene object, we lose access to it in the new scene.
		if (so->IsLocal())
			m_current_sceneobject.reset();
	}

	m_current_scene = new_scene;

	FollowedSceneObjects.clear();

	return old_scene;
}

std::weak_ptr<SceneObject> Player::SwitchControlledSceneObject(std::shared_ptr<SceneObject>& new_scene_object)
{
	auto old_so = m_current_sceneobject;

	m_current_sceneobject = new_scene_object;
	OnSwitchedControl.RunAll(new_scene_object, old_so);

	// If we are a guest, inform the host that we have changed our controlled scene object.
	if (m_server->IsGuest())
	{
		packet::SceneObjectControl packet;
		packet.set_player_id(m_player_id);
		packet.set_sceneobject_id(new_scene_object->ID);
		m_server->Send(network::PACKETID(network::Packets::SCENEOBJECTCONTROL), network::Channel::RELIABLE, packet);
	}

	log::PrintLine(":: Player {} taking control of scene object {}.", m_player_id, new_scene_object->ID);

	return old_so;
}

bool Player::HasItem(ItemID id) const
{
	return Account.Items.contains(id);
}

bool Player::HasItem(const std::string& name) const
{
	auto item_matches = [this, &name](const decltype(Account.Items)::value_type& item)
	{
		auto* base = item.second->ItemBase;
		if (base == nullptr) base = m_server->GetItemDefinition(item.second->ItemBaseID);
		if (base == nullptr) return false;
		return boost::iequals(base->Name, name);
	};

	return std::ranges::any_of(Account.Items, item_matches);
}

item::ItemInstancePtr Player::GetItem(ItemID id) const
{
	auto iter = Account.Items.find(id);
	if (iter == std::end(Account.Items))
		return nullptr;
	return iter->second;
}

std::set<item::ItemInstancePtr> Player::GetItems(const item::ItemDefinition* const item_definition) const
{
	std::set<item::ItemInstancePtr> results;
	if (item_definition == nullptr) return results;

	auto map_transform = [](const auto& item) { return item.second; };
	auto item_matches = [this, item_definition](const auto& item) { return item_definition->BaseID == item->ItemBaseID; };

	auto item_instance_range = Account.Items | std::views::transform(map_transform);
	std::ranges::copy_if(item_instance_range, std::inserter(results, std::end(results)), item_matches);

	return results;
}

std::set<item::ItemInstancePtr> Player::GetItems(const std::string& name) const
{
	std::set<item::ItemInstancePtr> results;

	auto map_transform = [](const auto& item) { return item.second; };
	auto item_matches = [this, &name](const item::ItemInstancePtr& item)
	{
		auto* base = item->ItemBase;
		if (base == nullptr) base = m_server->GetItemDefinition(item->ItemBaseID);
		if (base == nullptr) return false;
		return boost::iequals(base->Name, name);
	};

	auto item_instance_range = Account.Items | std::views::transform(map_transform);
	std::ranges::copy_if(item_instance_range, std::inserter(results, std::end(results)), item_matches);

	return results;
}

std::set<std::string> Player::GetAllItemTags() const
{
	std::set<std::string> results;

	std::ranges::for_each(Account.Items,
		[&results](const decltype(Account.Items)::value_type& pair)
		{
			if (pair.second->ItemBase == nullptr) return;
			std::ranges::for_each(pair.second->ItemBase->Tags, [&results](const auto& tag) { results.insert(tag); });
		});

	return results;
}


} // end namespace tdrp::server
