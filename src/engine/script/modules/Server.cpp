#include "engine/script/modules/bind.h"

#include "engine/server/Server.h"
#include "engine/scene/Scene.h"

namespace tdrp::script::modules
{

namespace functions
{
	static SceneObjectPtr create_scene_object(server::Server& server, SceneObjectType type, const std::string& object_class)
	{
		return server.CreateSceneObject(type, object_class);
	}

	static item::ItemInstancePtr give_item(server::Server& server, server::PlayerPtr player, ItemID baseId, item::ItemType itemType)
	{
		return server.GiveItemToPlayer(player, baseId, itemType);
	}

	static item::ItemInstancePtr give_item(server::Server& server, server::PlayerPtr player, item::ItemDefinition* item, item::ItemType itemType, size_t count)
	{
		if (item != nullptr)
			return server.GiveItemToPlayer(player, item->BaseID, itemType, count);
		return nullptr;
	}

	static item::ItemInstancePtr give_item(server::Server& server, server::PlayerPtr player, const std::string& itemName, item::ItemType itemType, size_t count)
	{
		auto item = server.GetItemDefinition(itemName);
		if (item == nullptr) return nullptr;

		return server.GiveItemToPlayer(player, item->BaseID, itemType, count);
	}

	static item::ItemInstancePtr give_item(server::Server& server, server::PlayerPtr player, const std::string& itemName, item::ItemType itemType)
	{
		return give_item(server, player, itemName, itemType, 1);
	}

	static item::ItemInstancePtr give_item(server::Server& server, server::PlayerPtr player, item::ItemDefinition* item, item::ItemType itemType)
	{
		return give_item(server, player, item, itemType, 1);
	}


	static item::ItemInstancePtr remove_item(server::Server& server, server::PlayerPtr player, ItemID id)
	{
		return server.RemoveItemFromPlayer(player, id);
	}

	static item::ItemInstancePtr remove_item(server::Server& server, server::PlayerPtr player, item::ItemInstancePtr item, size_t count)
	{
		if (item != nullptr)
			return server.RemoveItemFromPlayer(player, item->ID, count);
		return nullptr;
	}

	static item::ItemInstancePtr remove_item(server::Server& server, server::PlayerPtr player, item::ItemInstancePtr item)
	{
		return remove_item(server, player, item, 1);
	}

	static void remove_items(server::Server& server, server::PlayerPtr player, const std::string& itemName, size_t count)
	{
		auto items = player->GetItems(itemName);
		for (const auto& item : items)
			server.RemoveItemFromPlayer(player, item->ID, count);
	}

	static void remove_items(server::Server& server, server::PlayerPtr player, const std::string& itemName)
	{
		remove_items(server, player, itemName, 1);
	}

	static void send_event(server::Server& self, scene::ScenePtr scene, const std::string& name, const std::string& data, Vector2df origin, float radius)
	{
		self.SendEvent(scene, self.GetPlayer(), name, data, origin, radius);
	}

	static void send_level_event(server::Server& self, scene::ScenePtr scene, const std::string& name, const std::string& data)
	{
		self.SendEvent(scene, self.GetPlayer(), name, data);
	}

	static void send_server_event(server::Server& self, const std::string& name, const std::string& data)
	{
		self.SendEvent(nullptr, self.GetPlayer(), name, data);
	}

} // end namespace functions


void bind_server(sol::state& lua)
{
	lua.new_usertype<server::Server>("Server", sol::no_constructor,
		"Player", sol::readonly_property(&server::Server::GetPlayer),

		"OnStarted", sol::writeonly_property(&server::Server::SetOnStarted),
		"OnPlayerJoin", sol::writeonly_property(&server::Server::SetOnPlayerJoin),
		"OnPlayerLeave", sol::writeonly_property(&server::Server::SetOnPlayerLeave),
		"OnServerTick", sol::writeonly_property(&server::Server::SetOnServerTick),
		"OnServerEvent", sol::writeonly_property(&server::Server::SetOnServerEvent),
		"OnEvent", sol::writeonly_property(&server::Server::SetOnEvent),

		"IsHost", sol::readonly_property(&server::Server::IsHost),
		"IsGuest", sol::readonly_property(&server::Server::IsGuest),
		"IsSinglePlayer", sol::readonly_property(&server::Server::IsSinglePlayer),

		"GetScene", &server::Server::GetScene,
		"GetDefaultScene", [](server::Server& server) -> scene::ScenePtr { return server.GetScene(server.GetPackage()->GetStartingScene()); },

		"GiveClientScript", &server::Server::AddPlayerClientScript,
		"RemoveClientScript", &server::Server::RemovePlayerClientScript,

		"GetItemDefinition", sol::overload(
			sol::resolve<item::ItemDefinition*(ItemID)>(&server::Server::GetItemDefinition),
			sol::resolve<item::ItemDefinition*(const std::string&)>(&server::Server::GetItemDefinition)
		),
		"GiveItemToPlayer", sol::overload(
			sol::resolve<item::ItemInstancePtr(server::PlayerPtr, ItemID, item::ItemType, size_t)>(&server::Server::GiveItemToPlayer),
			sol::resolve<item::ItemInstancePtr(server::Server&, server::PlayerPtr, ItemID, item::ItemType)>(&functions::give_item),
			sol::resolve<item::ItemInstancePtr(server::Server&, server::PlayerPtr, item::ItemDefinition*, item::ItemType, size_t)>(&functions::give_item),
			sol::resolve<item::ItemInstancePtr(server::Server&, server::PlayerPtr, item::ItemDefinition*, item::ItemType)>(&functions::give_item),
			sol::resolve<item::ItemInstancePtr(server::Server&, server::PlayerPtr, const std::string&, item::ItemType, size_t)>(&functions::give_item),
			sol::resolve<item::ItemInstancePtr(server::Server&, server::PlayerPtr, const std::string&, item::ItemType)>(&functions::give_item)
		),
		"GiveVariantItemToPlayer", &server::Server::GiveVariantItemToPlayer,
		"RemoveItemFromPlayer", sol::overload(
			sol::resolve<item::ItemInstancePtr(server::PlayerPtr, ItemID, size_t)>(&server::Server::RemoveItemFromPlayer),
			sol::resolve<item::ItemInstancePtr(server::Server&, server::PlayerPtr, ItemID)>(&functions::remove_item),
			sol::resolve<item::ItemInstancePtr(server::Server&, server::PlayerPtr, item::ItemInstancePtr, size_t)>(&functions::remove_item),
			sol::resolve<item::ItemInstancePtr(server::Server&, server::PlayerPtr, item::ItemInstancePtr)>(&functions::remove_item)
		),
		"RemoveItemsFromPlayer", sol::overload(
			sol::resolve<void(server::Server&, server::PlayerPtr, const std::string&, size_t)>(&functions::remove_items),
			sol::resolve<void(server::Server&, server::PlayerPtr, const std::string&)>(&functions::remove_items)
		),

		"CreateSceneObject", sol::overload(
			&server::Server::CreateSceneObject,
			&functions::create_scene_object
		),
		"DeleteSceneObject", sol::resolve<bool(uint32_t)>(&server::Server::DeleteSceneObject),
		"DeleteSceneObject", sol::resolve<bool(SceneObjectPtr)>(&server::Server::DeleteSceneObject),
		"DeletePlayerOwnedSceneObjects", &server::Server::DeletePlayerOwnedSceneObjects,

		"GetSceneObject", &server::Server::GetSceneObjectById,

		"SwitchSceneObjectScene", &server::Server::SwitchSceneObjectScene,
		"SwitchPlayerScene", &server::Server::SwitchPlayerScene,
		"SwitchSceneObjectOwnership", &server::Server::SwitchSceneObjectOwnership,

		"SendEvent", &functions::send_event,
		"SendLevelEvent", &functions::send_level_event,
		"SendServerEvent", &functions::send_server_event
	);
}

} // end namespace tdrp::script::modules
