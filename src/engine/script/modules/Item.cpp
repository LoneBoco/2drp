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

	static auto getTags(item::ItemDefinition& item)
	{
		return sol::as_table(item.Tags);
	}

	static auto getTag(item::ItemDefinition& item)
	{
		auto result = [&item](size_t index)
		{
			return *std::next(std::begin(item.Tags), index);
		};

		return result;
	}

	static auto getTagSize(item::ItemDefinition& item)
	{
		auto result = [&item]() -> size_t 
		{
			return item.Tags.size();
		};

		return result;
	}

	static item::ItemDefinition* getItemDefinition(item::ItemInstance& item)
	{
		if (item.ItemBase != nullptr)
			return item.ItemBase;
		return nullptr;
	}

	static auto getInstanceName(item::ItemInstance& item)
	{
		if (item.ItemBase == nullptr) return std::string{};
		return item.ItemBase->Name;
	}

	static auto getInstanceDescription(item::ItemInstance& item)
	{
		if (item.ItemBase == nullptr) return std::string{};
		return item.ItemBase->Description;
	}

	static auto getInstanceImage(item::ItemInstance& item)
	{
		if (item.ItemBase == nullptr) return std::string{};
		return item.ItemBase->Image;
	}

	static auto getInstanceTags(item::ItemInstance& item)
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
		"Tags", sol::readonly_property(&functions::getTags),

		// Functions.
		"Use", [](item::ItemDefinition& item) { item.OnUsed.RunAll(nullptr); },

		// Callbacks.
		"OnCreated", sol::writeonly_property(&item::ItemDefinition::SetOnCreated),
		"OnAdded", sol::writeonly_property(&item::ItemDefinition::SetOnAdded),
		"OnRemoved", sol::writeonly_property(&item::ItemDefinition::SetOnRemoved),
		"OnUsed", sol::writeonly_property(&item::ItemDefinition::SetOnUsed)
		//"OnUpdate", sol::writeonly_property(&item::ItemDefinition::SetOnUpdate),
		//"OnSelected", sol::writeonly_property(&item::ItemDefinition::SetOnSelected),
		);

	lua.new_usertype<item::ItemInstance>("ItemInstance", sol::no_constructor,
		"ID", sol::readonly_property(&item::ItemInstance::ID),
		"BaseID", sol::readonly_property(&item::ItemInstance::ItemBaseID),
		"Base", sol::readonly_property(&functions::getItemDefinition),
		"Type", sol::readonly_property(&item::ItemInstance::Type),

		// From ItemDefinition but directly added here for convenience.
		"Name", sol::readonly_property(&functions::getInstanceName),
		"Description", sol::readonly_property(&functions::getInstanceDescription),
		"Image", sol::readonly_property(&functions::getInstanceImage),
		"Tags", sol::readonly_property(&functions::getInstanceTags),

		// Functions.
		"Use", [](item::ItemInstance& item) { if (item.ItemBase != nullptr) item.ItemBase->OnUsed.RunAll(item); },

		// Conversion.
		"As", item_as
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
