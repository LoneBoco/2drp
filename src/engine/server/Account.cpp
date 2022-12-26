#include "engine/server/Account.h"

#include "engine/server/Server.h"
#include "engine/server/Player.h"
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

	auto file = server->FileSystem.GetFile(filesystem::path("accounts") / server->GetUniqueId() / (name + ".txt"));
	if (!file)
	{
		log::PrintLine(":: Account {} does not exist.", name);
		return;
	}

	pugi::xml_document doc;
	doc.load(*file);

	// Attributes.
	auto node_attributes = doc.child("attributes");
	for (const auto& node_attribute : node_attributes.children("attribute"))
	{
		std::string attrname = node_attribute.attribute("name").as_string();
		std::string attrtype = node_attribute.attribute("type").as_string();
		std::string attrvalue = node_attribute.attribute("value").as_string();
		auto type = Attribute::TypeFromString(attrtype);
		if (type == AttributeType::INVALID)
			log::PrintLine("!! Account flag was invalid: {}.{}", name, attrname);
		Flags.AddAttribute(attrname, type, attrvalue);
	}

	// Scripts.
	auto node_scripts = doc.child("scripts");
	for (const auto& node_script : node_scripts.children("script"))
	{
		std::string attrname = node_script.attribute("name").as_string();
		ClientScripts.insert(attrname);
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

	// Assemble account file.
	pugi::xml_document doc;
	auto node_attributes = doc.append_child("attributes");
	for (const auto& attribute : Flags)
	{
		auto node_attribute = node_attributes.append_child("attribute");
		auto attrname = node_attribute.append_attribute("name");
		auto attrtype = node_attribute.append_attribute("type");
		auto attrvalue = node_attribute.append_attribute("value");
		attrname.set_value(attribute.second->GetName().c_str());
		attrtype.set_value(attribute.second->TypeAsString().data());
		attrvalue.set_value(attribute.second->GetString().c_str());
	}
	auto node_scripts = doc.append_child("scripts");
	for (const auto& script : ClientScripts)
	{
		auto node_script = node_scripts.append_child("script");
		auto attrname = node_script.append_attribute("name");
		attrname.set_value(script.c_str());
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

} // end namespace tdrp::server
