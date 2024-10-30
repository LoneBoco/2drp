#include "engine/script/modules/bind.h"

#include "engine/server/Account.h"
#include "engine/server/Player.h"
#include "engine/item/Item.h"

#include <ranges>


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

	scene::ScenePtr get_current_scene(const server::Player& player)
	{
		return player.GetCurrentScene().lock();
	}

	SceneObjectPtr get_current_controlled_scene_object(const server::Player& player)
	{
		return player.GetCurrentControlledSceneObject().lock();
	}

	auto getFirstItemByName(const server::Player& self, const std::string& name) -> item::ItemInstancePtr
	{
		auto items = self.GetItems(name);
		if (items.empty()) return nullptr;
		return (*items.begin());
	}

	auto getFirstItemByNameAndType(const server::Player& self, const std::string& name, item::ItemType type) -> item::ItemInstancePtr
	{
		auto items = self.GetItems(name);
		auto items_of_type = items
			| std::ranges::views::filter([type](const item::ItemInstancePtr& item) { return item->Type == type; })
			| std::ranges::views::take(1);
		if (items_of_type.empty()) return nullptr;
		return items_of_type.front();
	}

	auto getAllItems(const server::Player& self)
	{
		return sol::as_table(self.Account.Items);
	}

	auto getItemsByDefinition(const server::Player& self, item::ItemDefinition* item_definition)
	{
		auto items = self.GetItems(item_definition) | std::views::transform([](const auto& item) { return std::make_pair(item->ID, item); });
		std::map<ItemID, item::ItemInstancePtr> results{ std::ranges::begin(items), std::ranges::end(items) };
		return sol::as_table(results);
	}

	auto getItemsByName(const server::Player& self, const std::string& name)
	{
		auto items = self.GetItems(name) | std::views::transform([](const auto& item) { return std::make_pair(item->ID, item); });
		std::map<ItemID, item::ItemInstancePtr> results{ std::ranges::begin(items), std::ranges::end(items) };
		return sol::as_table(results);
	}

	auto getItemsMatchingF(const server::Player& self, sol::variadic_args va, bool all)
	{
		// Collect a list of search tags.
		std::unordered_set<std::string> search_tags;
		std::ranges::for_each(va, [&search_tags](sol::stack_proxy arg)
		{
			search_tags.insert(arg.get<std::string>());
		});

		auto by_tag = [&search_tags, all](const auto& pair) -> bool
		{
			const item::ItemInstancePtr& item = pair.second;
			if (item->ItemBase == nullptr) return false;
			return std::ranges::all_of(search_tags, [&item](const auto& tag) { return std::ranges::contains(item->ItemBase->Tags, tag); });
			return std::ranges::any_of(search_tags, [&item](const auto& tag) { return std::ranges::contains(item->ItemBase->Tags, tag); });
		};

		auto items = self.Account.Items
			| std::views::filter(by_tag);

		std::map<ItemID, item::ItemInstancePtr> results{ std::ranges::begin(items), std::ranges::end(items) };
		return sol::as_table(results);
	}

	auto getItemsMatchingTags(const server::Player& self, sol::variadic_args va)
	{
		// Collect a list of search tags.
		std::unordered_set<std::string> search_tags;
		std::ranges::for_each(va, [&search_tags](sol::stack_proxy arg)
			{
				search_tags.insert(arg.get<std::string>());
			});

		auto by_tag = [&search_tags](const auto& pair) -> bool
		{
			const item::ItemInstancePtr& item = pair.second;
			if (item->ItemBase == nullptr) return false;
			return std::ranges::any_of(search_tags, [&item](const auto& tag) { return std::ranges::contains(item->ItemBase->Tags, tag); });
		};

		auto items = self.Account.Items
			| std::views::filter(by_tag);

		std::map<ItemID, item::ItemInstancePtr> results{ std::ranges::begin(items), std::ranges::end(items) };
		return sol::as_table(results);
	}

	auto getItemsMatchingEveryTag(const server::Player& self, sol::variadic_args va)
	{
		// Collect a list of search tags.
		std::unordered_set<std::string> search_tags;
		std::ranges::for_each(va, [&search_tags](sol::stack_proxy arg)
			{
				search_tags.insert(arg.get<std::string>());
			});

		auto by_tag = [&search_tags](const auto& pair) -> bool
		{
			const item::ItemInstancePtr& item = pair.second;
			if (item->ItemBase == nullptr) return false;
			return std::ranges::all_of(search_tags, [&item](const auto& tag) { return std::ranges::contains(item->ItemBase->Tags, tag); });
		};

		auto items = self.Account.Items
			| std::views::filter(by_tag);

		std::map<ItemID, item::ItemInstancePtr> results{ std::ranges::begin(items), std::ranges::end(items) };
		return sol::as_table(results);
	}

	auto getAllItemTags(server::Player& self)
	{
		return sol::as_table(self.GetAllItemTags());
	}

} // end namespace functions


void bind_player(sol::state& lua)
{
	lua.new_usertype<server::Account>("Account", sol::no_constructor,
		"Scene", &server::Account::LastKnownScene,
		"Flags", &server::Account::Flags
	);

	lua.new_usertype<server::Player>("Player", sol::no_constructor,
		//"SwitchScene", &server::Player::SwitchScene,
		"SwitchControlledSceneObject", &server::Player::SwitchControlledSceneObject,
		"GetCurrentScene", &functions::get_current_scene,
		"GetCurrentControlledSceneObject", &functions::get_current_controlled_scene_object,
		"GetPlayerId", &server::Player::GetPlayerId,

		"HasItem", sol::overload(
			sol::resolve<bool(ItemID) const>(&server::Player::HasItem),
			sol::resolve<bool(const std::string&) const>(&server::Player::HasItem)
		),
		"GetItem", sol::overload(&server::Player::GetItem, &functions::getFirstItemByName, &functions::getFirstItemByNameAndType),
		"GetItems", sol::overload(&functions::getAllItems, &functions::getItemsByName, &functions::getItemsByDefinition),
		"GetItemsMatchingTags", &functions::getItemsMatchingTags,
		"GetItemsMatchingEveryTag", &functions::getItemsMatchingEveryTag,
		"GetAllItemTags", &functions::getAllItemTags,

		"Account", &server::Player::Account,

		sol::meta_function::equal_to, [](const server::Player& self, const server::Player& other) { return self.GetPlayerId() == other.GetPlayerId(); },

		"OnSwitchedControl", sol::writeonly_property(&server::Player::SetOnSwitchedControl)
	);
}

} // end namespace tdrp::script::modules
