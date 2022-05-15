#include "engine/loader/PackageLoader.h"

#include "engine/filesystem/File.h"

#include <format>
#include <string>
#include <map>
#include <pugixml.hpp>

namespace tdrp::loader
{

void LoadObjectClasses(server::Server& server, std::map<std::string, std::shared_ptr<ObjectClass>>& object_classes, std::shared_ptr<package::Package>& package, pugi::xml_node& node_doc)
{
	for (const auto& node_classes : node_doc.children("objectclasses"))
	{
		std::string_view attrib_file = node_classes.attribute("file").as_string();
		if (!attrib_file.empty())
		{
			auto classes = server.FileSystem.GetFile(package->GetBasePath(), attrib_file);
			if (classes != nullptr)
			{
				pugi::xml_document doc;
				doc.load(*classes);

				// Recursive!
				LoadObjectClasses(server, object_classes, package, doc);
			}
		}
		else
		{
			for (const auto& node_class : node_classes.children("class"))
			{
				std::string classname = node_class.attribute("name").as_string();
				auto pc = std::make_shared<ObjectClass>(classname);

				// Script.
				for (const auto& node_script : node_class.children("script"))
				{
					const auto& attribute_type = node_script.attribute("type");
					std::string* script = &pc->ScriptClient;
					if (boost::iequals(attribute_type.as_string(), "server"))
						script = &pc->ScriptServer;

					std::string file = node_script.attribute("file").as_string();
					if (!file.empty())
					{
						// Load script from file.
						auto scriptfile = server.FileSystem.GetFile(package->GetBasePath(), file);
						script->append(scriptfile->ReadAsString());
						script->append("\n");
					}
					else
					{
						// Load text directly.
						script->append(node_script.text().get());
						script->append("\n");
					}
				}

				// Attributes.
				for (const auto& node_attribute : node_class.children("attribute"))
				{
					std::string attrname = node_attribute.attribute("name").as_string();
					std::string attrtype = node_attribute.attribute("type").as_string();
					std::string attrvalue = node_attribute.attribute("value").as_string();
					pc->Attributes.AddAttribute(attrname, Attribute::TypeFromString(attrtype), attrvalue);
				}

				object_classes.insert(std::make_pair(classname, pc));
			}
		}
	}
}

void LoadScripts(server::Server& server, std::map<std::string, std::string>& scripts, std::shared_ptr<package::Package>& package, const std::string_view& node_name, pugi::xml_node& node_doc)
{
	for (const auto& node_scripts : node_doc.children(node_name.data()))
	{
		std::string_view attrib_file = node_scripts.attribute("file").as_string();
		if (!attrib_file.empty())
		{
			auto script_file = server.FileSystem.GetFile(package->GetBasePath(), attrib_file);
			if (script_file != nullptr)
			{
				pugi::xml_document doc;
				doc.load(*script_file);

				// Recursive!
				LoadScripts(server, scripts, package, node_name, doc);
			}
		}
		else
		{
			for (const auto& node_script : node_scripts.children("script"))
			{
				std::string file = node_script.attribute("file").as_string();
				if (!file.empty())
				{
					// Load script from file.
					auto scriptfile = server.FileSystem.GetFile(package->GetBasePath(), file);
					auto script = scriptfile->ReadAsString();
					scripts.insert(std::make_pair(file, script));
				}
				else
				{
					// Load text directly.
					auto script = node_script.text().get();

					// Generate a random name.
					std::string name = std::format("{}_{}", node_name, scripts.size() + 1);
					scripts.insert(std::make_pair(name, script));
				}
			}
		}
	}
}

std::pair<bool, std::shared_ptr<package::Package>> PackageLoader::LoadIntoServer(server::Server& server, const std::string& name)
{
	filesystem::path root_dir = filesystem::path("packages") / name;

	// Make sure our package directory exists.
	if (!filesystem::exists(root_dir))
		return std::make_pair(false, nullptr);

	// Bind the package directory to our server filesystem.
	server.FileSystem.Bind(root_dir, "levels");
	server.FileSystem.WaitUntilFilesSearched();

	// Create the package and save the directory it is under.
	auto package = std::make_shared<package::Package>(name);
	package->m_basepath = root_dir;

	std::string tileset_file{ "tilesets.xml" };

	// Load our package metadata.
	if (filesystem::exists(root_dir / "package.xml"))
	{
		fs::File packagefile{ root_dir / "package.xml" };
		pugi::xml_document doc;
		doc.load(packagefile);

		auto node_package = doc.child("package");
		if (!node_package.empty())
		{
			std::string name = node_package.attribute("name").as_string();
			std::string version = node_package.attribute("version").as_string();

			auto node_logo = node_package.child("logo");
			auto node_desc = node_package.child("description");
			auto node_tileset = node_package.child("tileset");
			auto node_startingscene = node_package.child("startingscene");
			auto node_loadingscene = node_package.child("loadingscene");

			package->m_name = name;
			package->m_version = version;
			if (!node_logo.empty())
				package->m_logo = node_logo.attribute("file").as_string();
			if (!node_desc.empty())
				package->m_description = node_desc.text().get();
			if (!node_tileset.empty())
				tileset_file = node_tileset.attribute("file").as_string();
			if (!node_startingscene.empty())
				package->m_starting_scene = node_startingscene.text().get();
			if (!node_loadingscene.empty())
				package->m_loading_scene = node_loadingscene.text().get();

			// Load our classes.
			LoadObjectClasses(server, server.m_object_classes, package, node_package);

			// Load our client scripts.
			LoadScripts(server, server.m_client_scripts, package, "clientscripts", node_package);
			LoadScripts(server, server.m_server_scripts, package, "serverscripts", node_package);
		}
	}

	// Load our tilesets.
	auto tilesets = server.FileSystem.GetFile(root_dir, tileset_file);
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
