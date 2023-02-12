#pragma once

#include <set>

#include "engine/common.h"

#include "engine/scene/ObjectAttributes.h"
#include "engine/script/Function.h"

namespace tdrp::item
{

enum class ItemType : uint8_t
{
	SINGLE = 0,
	STACKABLE,
	VARIANT,

	COUNT
};

using tag_container = std::vector<std::string>;


/// <summary>
/// The item's definition.
/// Actual items stored by the player are linked to this.
/// </summary>
class ItemDefinition
{
public:
	ItemDefinition() = delete;

	ItemDefinition(ItemID&& id, std::string&& name, std::string&& description, std::string&& image, std::string&& clientscript, tag_container&& tags)
		: BaseID{ id }, Name{ name }, Description{ description }, Image{ image }, ClientScript{ clientscript }, Tags{ tags }
	{}

public:
	ItemID BaseID;
	std::string Name;
	std::string Description;
	std::string Image;
	std::string ClientScript;
	tag_container Tags;
};
using ItemDefinitionUPtr = std::unique_ptr<ItemDefinition>;


/// <summary>
/// An instance of an item stored on the player's account.
/// </summary>
class ItemInstance
{
	SCRIPT_SETUP;
	SCRIPT_FUNCTION(OnCreated);
	//SCRIPT_FUNCTION(OnUpdate);
	SCRIPT_FUNCTION(OnAdded);
	SCRIPT_FUNCTION(OnRemoved);
	//SCRIPT_FUNCTION(OnSelected);
	SCRIPT_FUNCTION(OnUsed);

public:
	ItemInstance(ItemID id, ItemID baseId) : ID{ id }, ItemBaseID{ baseId }, Type{ ItemType::SINGLE } {}
	ItemInstance(ItemID id, ItemID baseId, ItemType type) : ID{ id }, ItemBaseID{ baseId }, Type{ type } {}
	virtual ~ItemInstance() {}

public:
	ItemID ID;
	ItemID ItemBaseID;
	ItemType Type;
	ItemDefinition* ItemBase = nullptr;
};
using ItemInstancePtr = std::shared_ptr<ItemInstance>;


/// <summary>
/// An item instance that can be stacked (has an item count).
/// </summary>
class ItemStackable : public ItemInstance
{
public:
	ItemStackable(ItemID id, ItemID baseId) : ItemInstance(id, baseId, ItemType::STACKABLE) {}
	virtual ~ItemStackable() {}

public:
	size_t Count = 0;
};


/// <summary>
/// An item that also has attribute differences.
/// </summary>
class ItemVariant : public ItemInstance
{
public:
	ItemVariant(ItemID id, ItemID baseId) : ItemInstance(id, baseId, ItemType::VARIANT) {}
	virtual ~ItemVariant() {}

public:
	ObjectAttributes VariantAttributes;
};


} // end namespace tdrp::item
