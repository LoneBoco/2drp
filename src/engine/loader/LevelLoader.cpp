#include "engine/loader/LevelLoader.h"

#include "engine/filesystem/File.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/scene/SceneObject.h"

#include <pugixml.hpp>

using namespace tdrp::scene;

namespace tdrp::loader
{

std::shared_ptr<tdrp::scene::Scene> LevelLoader::CreateScene(package::Package& package, const std::string level)
{
	auto base = package.GetBasePath();
	auto scene = std::make_shared<Scene>();
	int32_t version = 1;

	// Get our _info.xml file.
	fs::File info{ base / "levels" / level / "_info.xml" };
	if (!info)
		return scene;

	// Load our _info.xml file.
	{
		pugi::xml_document infodoc;
		infodoc.load(info);

		// Determine level version.
		auto n_level = infodoc.child("level");
		if (n_level)
			version = n_level.attribute("version").as_int(1);
	}

	// Load our chunks.
	for (auto& f : filesystem::directory_iterator(base / "levels" / level))
	{
		// If it is not a regular file, abort.
		if (!filesystem::is_regular_file(f.status()))
			continue;

		// If this is a chunk, load it!
		if (f.path().filename().generic_string().find("chunk") == 0)
		{
			pugi::xml_document chunkdoc;
			chunkdoc.load_file(f.path().c_str());

			// Load all of our scene objects.
			for (auto& sceneobjects : chunkdoc.children("sceneobjects"))
			{
				for (auto& object : sceneobjects)
				{
					std::string type = object.attribute("type").as_string("default");
					std::string scriptclass = object.attribute("class").as_string();

					// Load all properties.
					for (auto& prop : object.children("property"))
					{
						std::string name = prop.attribute("name").as_string();
						std::string type = prop.attribute("type").as_string();
						std::string value = prop.attribute("value").as_string();
					}
				}
			}
		}
	}

	return scene;
}

} // end namespace tdrp::loader
