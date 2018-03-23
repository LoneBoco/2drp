#include "engine/loader/PackageLoader.h"

#include "engine/filesystem/File.h"

#include <pugixml.hpp>

namespace tdrp::loader
{

std::shared_ptr<package::Package> PackageLoader::CreatePackage(const std::string& name)
{
	// Make sure our package directory exists.
	if (!filesystem::exists(filesystem::path("packages") / name))
		return std::shared_ptr<package::Package>(nullptr);

	auto package = std::make_shared<package::Package>(name);
	std::string object_class_file{ "classes.xml" };
	std::string client_script_file{ "client.js" };

	// Create our default "blank" class.
	package->m_object_classes.insert(std::make_pair("blank", std::make_shared<ObjectClass>("blank")));

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
			auto node_clientscript = node_package.child("clientscript");

			//if (!node_logo.empty())
			//	; // <logo file="logo.png" />
			if (!node_desc.empty())
				package->m_description = node_desc.text().get();
			if (!node_objectclass.empty())
				object_class_file = node_objectclass.attribute("file").as_string();
			if (!node_clientscript.empty())
				client_script_file = node_clientscript.attribute("file").as_string();
		}
	}

	// Load our client script.
	auto client_script = package->GetFileSystem().GetFile(client_script_file);
	if (client_script != nullptr)
	{
		// TODO: Load client script.
	}

	// Load our classes.
	auto classes = package->GetFileSystem().GetFile(object_class_file);
	if (classes != nullptr)
	{
		pugi::xml_document doc;
		doc.load(*classes);

		for (auto& node_classes : doc.children("objectclasses"))
		{
			for (auto& node_class : node_classes.children("class"))
			{
				std::string classname = node_class.attribute("name").as_string();
				auto pc = std::make_shared<ObjectClass>(classname);

				// Script.
				auto& node_script = node_class.child("script");
				if (!node_script.empty())
				{
					std::string file = node_script.attribute("file").as_string();
					if (!file.empty())
					{
						// Load script from file.
						auto scriptfile = package->GetFileSystem().GetFile(file);
					}
					else
					{
						// Load text directly.
						std::string script{ node_script.text().get() };
					}
				}

				// Attributes.
				for (auto& node_attribute : node_class.children("attribute"))
				{
					std::string attrname = node_attribute.attribute("name").as_string();
					std::string attrtype = node_attribute.attribute("type").as_string();
					std::string attrvalue = node_attribute.attribute("value").as_string();
					pc->Attributes.AddAttribute(name, Attribute::TypeFromString(attrtype), attrvalue);
				}

				package->m_object_classes.insert(std::make_pair(classname, pc));
			}
		}
	}

	return package;
}

} // end namespace tdrp::loader
