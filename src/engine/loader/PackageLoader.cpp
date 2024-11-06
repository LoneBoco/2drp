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
	static void LoadFileSystem(server::Server& server, fs::FileCategory category, const std::filesystem::path& package_path, const pugi::xml_node& node_fs)
	{
		if (auto dir = node_fs.attribute("directory"); !dir.empty())
		{
			server.FileSystem.Bind(category, package_path / dir.as_string());
		}

		for (const auto& dirs : node_fs.children("directory"))
		{
			server.FileSystem.Bind(category, package_path / dirs.text().get());
		}
	}

	static void LoadObjectClass(server::Server& server, std::unordered_map<std::string, std::shared_ptr<ObjectClass>>& object_classes, std::shared_ptr<package::Package>& package, const pugi::xml_node& node_class)
	{
		std::string classname = node_class.attribute("name").as_string();
		auto pc = std::make_shared<ObjectClass>(classname);

		// Script.
		for (const auto& node_script : node_class.children("script"))
		{
			const auto& attribute_type = node_script.attribute("type");
			std::string* script = &pc->ClientScript;
			if (boost::iequals(attribute_type.as_string(), "server"))
				script = &pc->ServerScript;

			std::string file = node_script.attribute("file").as_string();
			if (!file.empty())
			{
				// Load script from file.
				auto scriptfile = server.FileSystem.GetFile(fs::FileCategory::WORLD, file);
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

	static void LoadObjectClasses(server::Server& server, std::unordered_map<std::string, std::shared_ptr<ObjectClass>>& object_classes, std::shared_ptr<package::Package>& package, const pugi::xml_node& node_doc)
	{
		for (const auto& node_classes : node_doc.children("classes"))
		{
			std::string_view attrib_file = node_classes.attribute("file").as_string();
			if (!attrib_file.empty())
			{
				auto classes = server.FileSystem.GetFile(fs::FileCategory::CONFIG, attrib_file);
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
					std::string_view attrib_file = node_class.attribute("file").as_string();
					if (!attrib_file.empty())
					{
						// Load the class information from a file.
						if (auto classes = server.FileSystem.GetFile(fs::FileCategory::CONFIG, attrib_file); classes != nullptr)
						{
							pugi::xml_document doc;
							doc.load(*classes);

							if (const auto& file_node_class = doc.child("class"); !file_node_class.empty())
								LoadObjectClass(server, object_classes, package, file_node_class);
						}
					}
					else LoadObjectClass(server, object_classes, package, node_class);
				}
			}
		}
	}

	static void LoadScripts(server::Server& server, std::vector<std::pair<std::string, std::string>>& scripts, std::shared_ptr<package::Package>& package, const std::string_view& search_node_name, const pugi::xml_node& parent_node)
	{
		const auto& node = parent_node.child(search_node_name.data());
		if (node.empty())
			return;

		auto readFile = [&](const std::string_view& file)
		{
			if (file.empty()) return;
			auto scriptfile = server.FileSystem.GetFile(fs::FileCategory::WORLD, file);
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

	static void LoadItems(server::Server& server, std::shared_ptr<package::Package>& package)
	{
		auto readFile = [&](fs::FileCategory category, const std::string_view& filename) -> std::string
		{
			if (filename.empty()) return {};
			auto file = server.FileSystem.GetFile(category, filename);
			if (file)
				return file->ReadAsString();
			else return {};
		};

		auto loadItem = [&](const filesystem::path& filename) -> item::ItemDefinitionUPtr
		{
			if (filename.empty()) return nullptr;

			auto itemfile = server.FileSystem.GetFile(fs::FileCategory::ITEMS, filename);
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
					clientscript = readFile(fs::FileCategory::WORLD, attrib_file.as_string());
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

		for (const auto& file : server.FileSystem.GetFiles(fs::FileCategory::ITEMS))
		{
			auto item = loadItem(file);
			server.AddItemDefinition(std::move(item));
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
	//server.FileSystem.Bind(root_dir, "levels");
	//server.FileSystem.WaitUntilFilesSearched();

	// Create the package and save the directory it is under.
	auto package = std::make_shared<package::Package>(name);
	package->m_basepath = root_dir;

	//std::string tileset_file{ "tilesets.xml" };

	// Load our package metadata.
	if (filesystem::exists(root_dir / "package.xml"))
	{
		fs::File packagefile{ root_dir / "package.xml" };
		pugi::xml_document doc;
		doc.load(packagefile);

		auto node_package = doc.child("package");
		if (node_package.empty())
			return std::make_pair(false, nullptr);

		// Load the file systems.
		for (const auto& node_filesystem : node_package.children("filesystem"))
		{
			auto node_config = node_filesystem.child("config");
			auto node_fonts = node_filesystem.child("fonts");
			auto node_items = node_filesystem.child("items");
			auto node_ui = node_filesystem.child("ui");
			auto node_world = node_filesystem.child("world");

			if (!node_config.empty())
				helper::LoadFileSystem(server, fs::FileCategory::CONFIG, root_dir, node_config);
			if (!node_fonts.empty())
				helper::LoadFileSystem(server, fs::FileCategory::FONTS, root_dir, node_fonts);
			if (!node_items.empty())
				helper::LoadFileSystem(server, fs::FileCategory::ITEMS, root_dir, node_items);
			if (!node_ui.empty())
				helper::LoadFileSystem(server, fs::FileCategory::UI, root_dir, node_ui);
			if (!node_world.empty())
				helper::LoadFileSystem(server, fs::FileCategory::WORLD, root_dir, node_world);

			server.FileSystem.WaitUntilFilesSearched();
		}

		// Load the package details.
		if (const auto& node_details = node_package.child("details"); !node_details.empty())
		{
			auto node_logo = node_details.child("logo");
			auto node_name = node_details.child("name");
			auto node_desc = node_details.child("description");
			auto node_version = node_details.child("version");
			auto node_author = node_details.child("author");

			if (!node_name.empty())
				package->m_name = node_name.text().get();
			else package->m_name = name;

			if (!node_logo.empty())
				package->m_logo = node_logo.attribute("file").as_string();
			if (!node_desc.empty())
				package->m_description = node_desc.text().get();
			if (!node_version.empty())
				package->m_version = node_version.text().get();
			if (!node_author.empty())
				package->m_author = node_author.text().get();
		}
		else return std::make_pair(false, nullptr);

		// Load the scene details.
		if (const auto& node_scene = node_package.child("scene"); !node_scene.empty())
		{
			auto node_starting = node_scene.child("starting");
			auto node_loading = node_scene.child("loading");
			auto node_classes = node_scene.child("classes");

			if (!node_starting.empty())
				package->m_starting_scene = node_starting.text().get();
			if (!node_loading.empty())
				package->m_loading_scene = node_loading.text().get();

			// Load object classes.
			helper::LoadObjectClasses(server, server.m_object_classes, package, node_scene);
		}

		// Load the script details.
		if (const auto& node_scripts = node_package.child("scripts"); !node_scripts.empty())
		{
			// Control scripts.
			if (const auto& node_control = node_scripts.child("control"); !node_control.empty())
			{
				// Load client scripts.
				std::vector<script_pair> clientcontrolscripts;
				std::vector<script_pair> servercontrolscripts;
				helper::LoadScripts(server, clientcontrolscripts, package, "client", node_control);
				helper::LoadScripts(server, servercontrolscripts, package, "server", node_control);
				std::for_each(std::begin(clientcontrolscripts), std::end(clientcontrolscripts), [&server](const script_pair& pair) { server.m_client_control_script += pair.second; server.m_client_control_script += "\n"; });
				std::for_each(std::begin(servercontrolscripts), std::end(servercontrolscripts), [&server](const script_pair& pair) { server.m_server_control_script += pair.second; server.m_server_control_script += "\n"; });
			}

			// Load client scripts.
			std::vector<script_pair> clientscripts;
			helper::LoadScripts(server, clientscripts, package, "clientscripts", node_package);
			for (const auto& [name, script] : clientscripts)
			{
				server.LoadClientScript(name, script);
			}
		}

		// Load items.
		helper::LoadItems(server, package);
	}

	// Load our tilesets.
	/*
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
	*/

	return std::make_pair(true, package);
}

} // end namespace tdrp
