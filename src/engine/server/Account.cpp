#include "engine/server/Account.h"

#include "engine/server/Server.h"
#include "engine/server/Player.h"
#include "engine/item/Item.h"
#include "engine/filesystem/File.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/Log.h"

#include <pugixml.hpp>


namespace tdrp::server
{

void Account::Load(const std::string& name)
{
	if (m_player == nullptr) return;
	auto server = m_player->GetServer();
	if (server == nullptr) return;

	m_name = name;

	auto file = server->FileSystem.GetFile(fs::FileCategory::ACCOUNTS, (name + ".txt"));
	if (!file)
	{
		log::PrintLine(":: Account {} does not exist.", name);
		return;
	}

	pugi::xml_document doc;
	doc.load(*file);

	auto load_attribute = [&name](std::string_view description, const pugi::xml_node& parent_node, ObjectAttributes& container)
	{
		for (const auto& node_attribute : parent_node.children("attribute"))
		{
			std::string attrname = node_attribute.attribute("name").as_string();
			std::string attrtype = node_attribute.attribute("type").as_string();
			std::string attrvalue = node_attribute.attribute("value").as_string();
			auto type = Attribute::TypeFromString(attrtype);
			if (type == AttributeType::INVALID)
				log::PrintLine("!! Player {}: {} {} was invalid.", name, description, attrname);
			container.AddAttribute(attrname, type, attrvalue);
		}
	};

	// Last known scene.
	auto node_scene = doc.child("scene");
	if (!node_scene.empty())
	{
		auto attrname = node_scene.attribute("last-known").as_string();
		auto scene = server->GetScene(attrname);
		if (scene != nullptr)
		{
			LastKnownScene = scene->GetName();
			m_player->m_current_scene = scene;
		}
	}

	// Flags.
	auto node_flags = doc.child("flags");
	load_attribute("Account flag", node_flags, Flags);

	// Scripts.
	auto node_scripts = doc.child("scripts");
	for (const auto& node_script : node_scripts.children("script"))
	{
		std::string attrname = node_script.attribute("name").as_string();
		ClientScripts.insert(attrname);
	}

	// Items.
	auto id_counter = 0;
	auto node_items = doc.child("items");
	for (const auto& node_item : node_items.children("item"))
	{
		auto attrid = id_counter++;
		// auto attrid = node_item.attribute("id").as_uint();
		auto attrbaseid = node_item.attribute("baseid").as_uint();
		auto attrtype = node_item.attribute("type").as_uint();
		if (attrtype >= static_cast<uint8_t>(item::ItemType::COUNT))
			continue;

		auto baseId = attrbaseid;
		auto itemType = static_cast<item::ItemType>(attrtype);
		switch (itemType)
		{
			case item::ItemType::SINGLE:
			{
				auto instance = std::make_shared<item::ItemInstance>(attrid, baseId);
				instance->ItemBase = server->GetItemDefinition(baseId);
				Items.insert(std::make_pair(attrid, instance));
				break;
			}
			case item::ItemType::STACKABLE:
			{
				auto node_stackable = node_item.child("stackable");
				if (node_stackable.empty()) break;
			
				auto attrcount = node_stackable.attribute("count").as_uint();
				auto stackable = std::make_shared<item::ItemStackable>(attrid, baseId);
				stackable->Count = attrcount;

				auto instance = std::dynamic_pointer_cast<item::ItemInstance>(stackable);
				instance->ItemBase = server->GetItemDefinition(baseId);
				Items.insert(std::make_pair(attrid, instance));
				break;
			}
			case item::ItemType::VARIANT:
			{
				auto node_variant = node_item.child("variant");
				if (node_variant.empty()) break;

				auto variant = std::make_shared<item::ItemVariant>(attrid, baseId);
				load_attribute(std::format("Item {} variant attribute", baseId), node_variant, variant->VariantAttributes);

				auto instance = std::dynamic_pointer_cast<item::ItemInstance>(variant);
				instance->ItemBase = server->GetItemDefinition(baseId);
				Items.insert(std::make_pair(attrid, instance));
				break;
			}
		}
	}
}

void Account::Save()
{
	if (m_player == nullptr) return;
	auto server = m_player->GetServer();
	if (server == nullptr) return;

	if (m_name.empty())
	{
		log::PrintLine("!! Failed to save account - name was empty.");
		return;
	}

	auto create_attribute = [](pugi::xml_node& parent_node, const Attribute& attribute)
	{
		auto node_attribute = parent_node.append_child("attribute");
		auto attrname = node_attribute.append_attribute("name");
		auto attrtype = node_attribute.append_attribute("type");
		auto attrvalue = node_attribute.append_attribute("value");
		attrname.set_value(attribute.Name.c_str());
		attrtype.set_value(attribute.TypeAsString().data());
		attrvalue.set_value(attribute.GetAs<std::string>().c_str());
	};

	pugi::xml_document doc;

	// Last known scene.
	auto node_scene = doc.append_child("scene");
	if (!node_scene.empty())
	{
		auto attrname = node_scene.append_attribute("last-known");
		auto scene = m_player->GetCurrentScene().lock();
		if (scene)
			attrname.set_value(scene->GetName().c_str());
	}

	// Assemble flags.
	auto node_flags = doc.append_child("flags");
	for (const auto& attribute : Flags)
	{
		create_attribute(node_flags, *attribute.second);
	}

	// Assemble scripts.
	auto node_scripts = doc.append_child("scripts");
	for (const auto& script : ClientScripts)
	{
		auto node_script = node_scripts.append_child("script");
		auto attrname = node_script.append_attribute("name");
		attrname.set_value(script.c_str());
	}

	// Assemble items.
	auto node_items = doc.append_child("items");
	for (const auto& [key, item] : Items)
	{
		auto node_item = node_items.append_child("item");
		// auto attrid = node_item.append_attribute("id");
		auto attrbaseid = node_item.append_attribute("baseid");
		auto attrtype = node_item.append_attribute("type");
		// attrid.set_value(item->ID);
		attrbaseid.set_value(item->ItemBaseID);
		attrtype.set_value(static_cast<uint8_t>(item->Type));

		// Stackable.
		if (auto stackable = std::dynamic_pointer_cast<item::ItemStackable>(item); stackable)
		{
			auto node_stackable = node_item.append_child("stackable");
			auto attrcount = node_stackable.append_attribute("count");
			attrcount.set_value(stackable->Count);
		}

		// Variant.
		if (auto variant = std::dynamic_pointer_cast<item::ItemVariant>(item); variant)
		{
			auto node_variant = node_item.append_child("variant");
			for (const auto& attribute : variant->VariantAttributes)
			{
				create_attribute(node_variant, *attribute.second);
			}
		}
	}

	// Create directories that don't exist.
	auto basepath{ filesystem::path("accounts") / server->GetUniqueId() };
	filesystem::create_directories(basepath);

	// Save account file.
	auto filename{ basepath / (m_name + ".txt") };
	doc.save_file(filename.c_str(), "\t");

	log::PrintLine(":: Saved account {}.", m_name);
}

void Account::AddClientScript(const std::string& name)
{
	ClientScripts.insert(name);
}

void Account::RemoveClientScript(const std::string& name)
{
	ClientScripts.erase(name);
}

void Account::AddItem(item::ItemInstancePtr item)
{
	Items.insert(std::make_pair(item->ID, item));
}

void Account::RemoveItem(item::ItemInstancePtr item)
{
	Items.erase(item->ID);
}

} // end namespace tdrp::server
