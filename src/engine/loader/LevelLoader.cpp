#include "engine/loader/LevelLoader.h"

#include "engine/filesystem/File.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/scene/SceneObject.h"

#include "engine/helper/base64.h"

#include <pugixml.hpp>

#include <boost/range/algorithm_ext.hpp>

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
				for (auto& object : sceneobjects.children("object"))
				{
					std::string scriptclass = object.attribute("class").as_string();
					auto c = package.GetObjectClass(scriptclass);
					if (c == nullptr)
						c = package.GetObjectClass("blank");

					uint32_t id = package.GetNextID();

					// Get the type of scene object we are loading.
					std::string type = object.attribute("type").as_string("default");
					SceneObjectType sotype = SceneObjectType::DEFAULT;
					if (boost::iequals(type, "static"))
						sotype = SceneObjectType::STATIC;
					else if (boost::iequals(type, "animated"))
						sotype = SceneObjectType::ANIMATED;
					else if (boost::iequals(type, "tiled"))
						sotype = SceneObjectType::TILED;

					// Create our scene object.
					std::shared_ptr<SceneObject> so = nullptr;
					switch (sotype)
					{
						case SceneObjectType::STATIC: so = std::make_shared<StaticSceneObject>(c, id); break;
						case SceneObjectType::ANIMATED: so = std::make_shared<AnimatedSceneObject>(c, id); break;
						case SceneObjectType::TILED: so = std::make_shared<TiledSceneObject>(c, id); break;
						default: so = std::make_shared<SceneObject>(c, id); break;
					}

					// Load all properties.
					for (auto& prop : object.children("property"))
					{
						std::string name = prop.attribute("name").as_string();
						std::string type = prop.attribute("type").as_string();
						std::string value = prop.attribute("value").as_string();
						so->Properties.Get(name)->SetAsType(Attribute::TypeFromString(type), value);
					}

					// Load all attributes.
					for (auto& prop : object.children("attribute"))
					{
						std::string name = prop.attribute("name").as_string();
						std::string type = prop.attribute("type").as_string();
						std::string value = prop.attribute("value").as_string();
						so->Attributes.AddAttribute(name, Attribute::TypeFromString(type), value);
					}

					// Load custom features.
					if (sotype == SceneObjectType::TILED)
					{
						auto sop = so.get();
						TiledSceneObject* tiled_so = dynamic_cast<TiledSceneObject*>(sop);
						auto& tileset = object.child("tileset");
						auto& tiledata = object.child("tiledata");

						// Tileset
						{
							int width = tileset.attribute("tile_w").as_int();
							int height = tileset.attribute("tile_h").as_int();
							std::string file = tileset.attribute("file").as_string();
							tiled_so->Tileset_File = file;
							tiled_so->Tileset_Tile_Dimension.x = width;
							tiled_so->Tileset_Tile_Dimension.y = height;
						}

						// Tiles
						{
							int width = tiledata.attribute("width").as_int();
							int height = tiledata.attribute("height").as_int();
							std::string format = tiledata.attribute("format").as_string();

							tiled_so->Tile_Data.clear();
							tiled_so->Tile_Dimension.x = width;
							tiled_so->Tile_Dimension.y = height;

							bool tdrp_format = true;
							if (boost::iequals(format, "graalnw"))
								tdrp_format = false;

							if (tdrp_format)
							{
								std::vector<uint8_t> tiles = base64::from(tiledata.text().get());
								if (tiles.size() == (width * height * 2))
								{
									for (size_t i = 0; i < tiles.size(); i += 2)
									{
										tiled_so->Tile_Data.push_back(tiles[i]);
										tiled_so->Tile_Data.push_back(tiles[i + 1]);
									}
								}
							}
							else
							{
								const std::string base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
								std::string tiles{ tiledata.text().get() };
								boost::remove_erase_if(tiles, boost::is_space());

								if (tiles.size() == (width * height * 2))
								{
									tiled_so->Tile_Data.reserve(width * height * 2);
									for (size_t i = 0; i < tiles.size(); i += 2)
									{
										uint16_t tile = (uint16_t)((base64.find(tiles[i]) << 6) + base64.find(tiles[i + 1]));
										uint8_t x = ((tile / 512) * 16) + tile % 16;
										uint8_t y = (tile % 512) / 16;

										tiled_so->Tile_Data.push_back(x);
										tiled_so->Tile_Data.push_back(y);
									}
								}
							}
						}
					}

					// Add the object to the scene.
					scene->AddObject(so);
				}
			}
		}
	}

	return scene;
}

} // end namespace tdrp::loader
