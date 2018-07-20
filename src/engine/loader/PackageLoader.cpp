#include "engine/loader/PackageLoader.h"

#include "engine/filesystem/File.h"

#include <pugixml.hpp>

namespace tdrp::loader
{

std::pair<bool, std::shared_ptr<package::Package>> PackageLoader::LoadIntoServer(server::Server& server, const std::string& name)
{
	// Make sure our package directory exists.
	if (!filesystem::exists(filesystem::path("packages") / name))
		return std::make_pair(false, nullptr);

	auto package = std::make_shared<package::Package>(name);
	std::string object_class_file{ "classes.xml" };
	std::string tileset_file{ "tilesets.xml" };
	std::string client_script_file{ "client.js" };

	// Load our package metadata.
	if (filesystem::exists(package->GetBasePath() / "package.xml"))
	{
		fs::File packagefile{ package->GetBasePath() / "package.xml" };
		pugi::xml_document doc;
		doc.load(packagefile);

		auto node_package = doc.child("package");
		if (!node_package.empty())
		{
			std::string name = node_package.attribute("name").as_string();
			std::string version = node_package.attribute("version").as_string();

			auto node_logo = node_package.child("logo");
			auto node_desc = node_package.child("description");
			auto node_objectclass = node_package.child("objectclass");
			auto node_tileset = node_package.child("tileset");
			auto node_clientscript = node_package.child("clientscript");
			auto node_startingscene = node_package.child("startingscene");

			if (!node_logo.empty())
				package->m_logo = node_logo.attribute("file").as_string();
			if (!node_desc.empty())
				package->m_description = node_desc.text().get();
			if (!node_objectclass.empty())
				object_class_file = node_objectclass.attribute("file").as_string();
			if (!node_tileset.empty())
				tileset_file = node_tileset.attribute("file").as_string();
			if (!node_clientscript.empty())
				client_script_file = node_clientscript.attribute("file").as_string();
			if (!node_startingscene.empty())
				package->m_starting_scene = node_startingscene.text().get();
		}
	}

	// Load our client script.
	auto client_script = package->GetFileSystem().GetFile(client_script_file);
	if (client_script != nullptr)
	{
		server.m_client_script = client_script->ReadAsString();
	}

	// Load our classes.
	auto classes = package->GetFileSystem().GetFile(object_class_file);
	if (classes != nullptr)
	{
		pugi::xml_document doc;
		doc.load(*classes);

		for (const auto& node_classes : doc.children("objectclasses"))
		{
			for (const auto& node_class : node_classes.children("class"))
			{
				std::string classname = node_class.attribute("name").as_string();
				auto pc = std::make_shared<ObjectClass>(classname);

				// Script.
				for (const auto& node_scripts : node_class.children("scripts"))
				{
					for (const auto& node_script : node_scripts.children("script"))
					{
						const auto& attribute_type = node_script.attribute("type");
						std::string* script = &pc->ScriptClient;
						if (boost::iequals(attribute_type.as_string(), "server"))
							script = &pc->ScriptServer;

						std::string file = node_script.attribute("file").as_string();
						if (!file.empty())
						{
							// Load script from file.
							auto scriptfile = package->GetFileSystem().GetFile(file);
							*script = scriptfile->ReadAsString();
						}
						else
						{
							// Load text directly.
							script->assign(node_script.text().get());
						}
					}
				}

				// Attributes.
				for (const auto& node_attribute : node_class.children("attribute"))
				{
					std::string attrname = node_attribute.attribute("name").as_string();
					std::string attrtype = node_attribute.attribute("type").as_string();
					std::string attrvalue = node_attribute.attribute("value").as_string();
					pc->Attributes.AddAttribute(name, Attribute::TypeFromString(attrtype), attrvalue);
				}

				server.m_object_classes.insert(std::make_pair(classname, pc));
			}
		}
	}

	// Load our tilesets.
	auto tilesets = package->GetFileSystem().GetFile(tileset_file);
	if (tilesets != nullptr)
	{
		pugi::xml_document doc;
		doc.load(*tilesets);

		for (const auto& node_tilesets : doc.children("tilesets"))
		{
			for (const auto& node_tileset : node_tilesets.children("tileset"))
			{
				auto t = std::make_shared<scene::Tileset>();

				// File.
				// TODO: Throw error if empty.
				const auto& node_file = node_tileset.child("file");
				if (node_file.empty())
					continue;

				t->File = node_file.text().get();

				// Tile dimensions.
				const auto& node_tiledimension = node_tileset.child("tiledimension");
				if (!node_tiledimension.empty())
				{
					t->TileDimensions.x = std::max(16, node_tiledimension.attribute("x").as_int());
					t->TileDimensions.y = std::max(16, node_tiledimension.attribute("y").as_int());
				}

				server.m_tilesets.insert(std::make_pair(t->File, t));
			}
		}
	}

	return std::make_pair(true, package);
}

} // end namespace tdrp::loader
