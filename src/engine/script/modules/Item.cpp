#include "engine/script/modules/bind.h"

#include "engine/item/Item.h"
#include "engine/scene/ObjectAttributes.h"


namespace sol
{
	template <>
	struct is_container<tdrp::ObjectAttributes> : std::false_type {};
}


namespace tdrp::script::modules
{

namespace functions
{
	template <typename T>
	T* convert(item::ItemInstance& item)
	{
		auto test = dynamic_cast<T*>(&item);
		if (test == nullptr)
			return nullptr;

		return test;
	}

	#define CONVERSION(T) static_cast<T* (*)(item::ItemInstance&)>(&functions::convert)

	auto getTags(item::ItemDefinition& item)
	{
		return sol::as_table(item.Tags);
	}

	auto getTag(item::ItemDefinition& item)
	{
		auto result = [&item](size_t index)
		{
			return *std::next(std::begin(item.Tags), index);
		};

		return result;
	}

	auto getTagSize(item::ItemDefinition& item)
	{
		auto result = [&item]() -> size_t 
		{
			return item.Tags.size();
		};

		return result;
	}

	auto getInstanceName(item::ItemInstance& item)
	{
		if (item.ItemBase == nullptr) return std::string{};
		return item.ItemBase->Name;
	}

	auto getInstanceDescription(item::ItemInstance& item)
	{
		if (item.ItemBase == nullptr) return std::string{};
		return item.ItemBase->Description;
	}

	auto getInstanceImage(item::ItemInstance& item)
	{
		if (item.ItemBase == nullptr) return std::string{};
		return item.ItemBase->Image;
	}

	auto getInstanceTags(item::ItemInstance& item)
	{
		if (item.ItemBase != nullptr)
			return sol::as_table(item.ItemBase->Tags);

		return sol::as_table(item::tag_container{});
	}
} // end namespace functions


void bind_item(sol::state& lua)
{
	lua.new_enum("ItemType",
		"SINGLE", item::ItemType::SINGLE,
		"STACKABLE", item::ItemType::STACKABLE,
		"VARIANT", item::ItemType::VARIANT
	);

	auto item_as = lua.create_table();
	item_as["Stackable"] = CONVERSION(item::ItemStackable);
	item_as["Variant"] = CONVERSION(item::ItemVariant);

	lua.new_usertype<item::ItemDefinition>("ItemDefinition", sol::no_constructor,
		"BaseID", sol::readonly_property(&item::ItemDefinition::BaseID),
		"Name", sol::readonly_property(&item::ItemDefinition::Name),
		"Description", sol::readonly_property(&item::ItemDefinition::Description),
		"Image", sol::readonly_property(&item::ItemDefinition::Image),
		"Tags", sol::readonly_property(&functions::getTags)
		);

	lua.new_usertype<item::ItemInstance>("ItemInstance", sol::no_constructor,
		"ID", sol::readonly_property(&item::ItemInstance::ID),
		"BaseID", sol::readonly_property(&item::ItemInstance::ItemBaseID),
		"Type", sol::readonly_property(&item::ItemInstance::Type),

		// From ItemDefinition but directly added here for convenience.
		"Name", sol::readonly_property(&functions::getInstanceName),
		"Description", sol::readonly_property(&functions::getInstanceDescription),
		"Image", sol::readonly_property(&functions::getInstanceImage),
		"Tags", sol::readonly_property(&functions::getInstanceTags),

		// Functions.
		"Use", [](item::ItemInstance& item) { item.OnUsed.RunAll(); },

		// Conversion.
		"As", item_as,

		// Callbacks.
		"OnCreated", sol::writeonly_property(&item::ItemInstance::SetOnCreated),
		//"OnUpdate", sol::writeonly_property(&item::ItemInstance::SetOnUpdate),
		"OnAdded", sol::writeonly_property(&item::ItemInstance::SetOnAdded),
		"OnRemoved", sol::writeonly_property(&item::ItemInstance::SetOnRemoved),
		//"OnSelected", sol::writeonly_property(&item::ItemInstance::SetOnSelected),
		"OnUsed", sol::writeonly_property(&item::ItemInstance::SetOnUsed)
		);

	lua.new_usertype<item::ItemStackable>("ItemStackable", sol::no_constructor,
		"Count", &item::ItemStackable::Count,

		sol::base_classes, sol::bases<item::ItemInstance>()
		);

	lua.new_usertype<item::ItemVariant>("ItemVariant", sol::no_constructor,
		"Attributes", &item::ItemVariant::VariantAttributes,

		sol::base_classes, sol::bases<item::ItemInstance>()
		);
}

} // end namespace tdrp::script::modules
