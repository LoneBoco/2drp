#include "engine/loader/Loader.h"

#include "engine/filesystem/File.h"
#include "engine/package/Package.h"
#include "engine/server/Server.h"
#include "engine/item/Item.h"

#include <format>
#include <string>
#include <map>
#include <vector>
#include <string_view>
#include <set>

#include <pugixml.hpp>

namespace tdrp::helper
{

	void LoadObjectClasses(server::Server& server, std::unordered_map<std::string, std::shared_ptr<ObjectClass>>& object_classes, std::shared_ptr<package::Package>& package, pugi::xml_node& node_doc)
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
						bool attrreplicated = node_attribute.attribute("replicated").as_bool(true);
						int32_t attrupdaterate = node_attribute.attribute("updaterate").as_int();

						auto type = Attribute::TypeFromString(attrtype);
						if (type == AttributeType::INVALID)
							log::PrintLine("!! Attribute type was invalid, skipping: {}.{}", classname, attrname);
						else
						{
							auto attrib = pc->Attributes.AddAttribute(attrname, type, attrvalue);
							attrib->Replicated = attrreplicated;
							attrib->NetworkUpdateRate = std::chrono::milliseconds{ attrupdaterate };
						}
					}

					object_classes.insert(std::make_pair(classname, pc));
				}
			}
		}
	}

	void LoadScripts(server::Server& server, std::vector<std::pair<std::string, std::string>>& scripts, std::shared_ptr<package::Package>& package, const std::string_view& search_node_name, pugi::xml_node& parent_node)
	{
		const auto& node = parent_node.child(search_node_name.data());
		if (node.empty())
			return;

		auto readFile = [&](const std::string_view& file)
		{
			if (file.empty()) return;
			auto scriptfile = server.FileSystem.GetFile(package->GetBasePath(), file);
			if (scriptfile) scripts.push_back(std::make_pair(std::string{ scriptfile->FilePath().stem().string() }, std::move(scriptfile->ReadAsString())));
		};

		auto readDirectory = [&](const std::string_view& directory)
		{
			if (directory.empty()) return;
			for (auto& file : filesystem::directory_iterator(package->GetBasePath() / directory))
			{
				if (!filesystem::is_regular_file(file.status()))
					continue;
				
				fs::File f{ file.path() };
				auto script = f.ReadAsString();
				if (!script.empty())
					scripts.emplace_back(std::make_pair(file.path().stem().string(), std::move(script)));
			}
		};

		// Node attributes.
		{
			std::string_view attrib_file = node.attribute("file").as_string();
			std::string_view attrib_directory = node.attribute("directory").as_string();
			if (!attrib_file.empty()) readFile(attrib_file);
			if (!attrib_directory.empty()) readDirectory(attrib_directory);
		}

		// Inline scripts.
		for (const auto& child : node.children("script"))
		{
			std::string_view text{ child.text().as_string() };
			if (!text.empty())
				scripts.emplace_back(std::make_pair("", std::string{ text }));
		}

		// Single files.
		for (const auto& child : node.children("file"))
		{
			std::string_view text{ child.text().as_string() };
			if (!text.empty())
				readFile(text);
		}

		// All files in directory.
		for (const auto& child : node.children("directory"))
		{
			std::string_view text{ child.text().as_string() };
			if (!text.empty())
				readDirectory(text);
		}
	}

	void LoadItems(server::Server& server, std::shared_ptr<package::Package>& package, pugi::xml_node& parent_node)
	{
		auto readFile = [&](const std::string_view& filename) -> std::string
		{
			if (filename.empty()) return {};
			auto file = server.FileSystem.GetFile(package->GetBasePath(), filename);
			if (file)
				return file->ReadAsString();
			else return {};
		};

		auto readDirectory = [&](const std::string_view& directory) -> std::vector<std::string>
		{
			std::vector<std::string> files;
			if (directory.empty()) return files;
			for (auto& file : filesystem::directory_iterator(package->GetBasePath() / directory))
			{
				if (!filesystem::is_regular_file(file.status()))
					continue;
				files.emplace_back((directory / file.path().filename()).string());
			}
			return files;
		};

		auto loadItem = [&](const filesystem::path& filename) -> item::ItemDefinitionUPtr
		{
			if (filename.empty()) return nullptr;

			auto itemfile = server.FileSystem.GetFile(package->GetBasePath(), filename);
			if (!itemfile) return nullptr;

			pugi::xml_document doc;
			doc.load(*itemfile);

			auto node_item = doc.child("item");
			if (node_item.empty()) return nullptr;

			auto node_id = node_item.child("id");
			auto node_name = node_item.child("name");
			auto node_description = node_item.child("description");
			auto node_image = node_item.child("image");
			auto node_clientscript = node_item.child("clientscript");
			auto node_tags = node_item.child("tags");

			if (node_id.empty())
			{
				log::PrintLine("!! Item {} is missing an id, skipping.", node_name.text().as_string());
				return nullptr;
			}

			// Load script.
			std::string clientscript;
			if (!node_clientscript.empty())
			{
				if (auto attrib_file = node_clientscript.attribute("file"); attrib_file)
					clientscript = readFile(attrib_file.as_string());
				else
					clientscript = node_clientscript.text().as_string();
			}

			// Load tags.
			std::set<std::string> tags;
			for (const auto& node_tag : node_tags.children("tag"))
			{
				tags.emplace(node_tag.text().as_string());
			}

			auto item = std::make_unique<item::ItemDefinition>(node_id.text().as_int(), node_name.text().as_string(), node_description.text().as_string(), node_image.text().as_string(), std::move(clientscript), item::tag_container{ tags.begin(), tags.end() });
			return item;
		};

		auto loadOneItem = [&](std::string_view filename)
		{
			auto item = loadItem(filename);
			server.AddItemDefinition(std::move(item));
		};

		auto loadManyItems = [&](std::string_view directory)
		{
			auto files = readDirectory(directory);
			for (const auto& file : files)
			{
				auto item = loadItem(file);
				server.AddItemDefinition(std::move(item));
			}
		};

		if (parent_node.empty())
			return;

		// Attributes.
		auto attrib_file = parent_node.attribute("file");
		auto attrib_directory = parent_node.attribute("directory");
		if (!attrib_file.empty()) loadOneItem(attrib_file.as_string());
		if (!attrib_directory.empty()) loadManyItems(attrib_directory.as_string());

		// File.
		for (const auto& node_file : parent_node.children("file"))
		{
			loadOneItem(node_file.text().as_string());
		}

		// Directory.
		for (const auto& node_directory : parent_node.children("directory"))
		{
			loadManyItems(node_directory.text().as_string());
		}
	}

} // end namespace tdrp::helper

namespace tdrp
{

std::pair<bool, std::shared_ptr<package::Package>> Loader::LoadPackageIntoServer(server::Server& server, const std::string& name)
{
	using script_pair = std::pair<std::string, std::string>;

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
			auto node_loadclientscripts = node_package.child("loadclientscripts");
			auto node_loaditems = node_package.child("loaditems");

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

			// Load object classes.
			helper::LoadObjectClasses(server, server.m_object_classes, package, node_package);

			// Load client scripts.
			std::vector<script_pair> clientcontrolscripts;
			std::vector<script_pair> servercontrolscripts;
			helper::LoadScripts(server, clientcontrolscripts, package, "clientcontrolscripts", node_package);
			helper::LoadScripts(server, servercontrolscripts, package, "servercontrolscripts", node_package);
			std::for_each(std::begin(clientcontrolscripts), std::end(clientcontrolscripts), [&server](const script_pair& pair) { server.m_client_control_script += pair.second; server.m_client_control_script += "\n"; });
			std::for_each(std::begin(servercontrolscripts), std::end(servercontrolscripts), [&server](const script_pair& pair) { server.m_server_control_script += pair.second; server.m_server_control_script += "\n"; });

			// Load client scripts.
			std::vector<script_pair> clientscripts;
			helper::LoadScripts(server, clientscripts, package, "loadclientscripts", node_package);
			for (const auto& [name, script] : clientscripts)
			{
				server.LoadClientScript(name, script);
			}

			// Load items.
			helper::LoadItems(server, package, node_loaditems);
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

} // end namespace tdrp
