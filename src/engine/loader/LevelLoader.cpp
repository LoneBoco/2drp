#include <charconv>
#include <concepts>
#include <ranges>

#include "engine/loader/Loader.h"

#include "engine/filesystem/File.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/scene/SceneObject.h"
#include "engine/scene/Scene.h"
#include "engine/scene/Tileset.h"
#include "engine/server/Server.h"

#include "engine/helper/base64.h"

#include <pugixml.hpp>
#include <boost/range/algorithm_ext.hpp>

#include <PlayRho/PlayRho.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <clipper2/clipper.h>
#include <polypartition.h>


using namespace tdrp::scene;

namespace tdrp::helper
{

	constexpr auto allowPhysicsNode = std::to_array<SceneObjectType>({
		SceneObjectType::STATIC,
		SceneObjectType::ANIMATED,
		SceneObjectType::TEXT
	});

	template <typename T>
	requires std::integral<T> || std::floating_point<T>
	std::vector<T> string_split_to_numbers(const std::string_view& sv)
	{
		std::vector<T> result;

		//auto split_view = std::views::common(sv | std::views::split(' '));
		auto split_view = sv | std::views::split(' ');
		for (const auto& split : split_view)
		{
			T value{};
			std::from_chars(split.data(), split.data() + split.size(), value);
			result.push_back(value);
		}

		return result;
	}

	static physics::BodyConfiguration createPhysicsBodyConfiguration(SceneObject* so, physics::BodyTypes bodytype, uint8_t ppu)
	{
		auto position = so->GetPosition();

		physics::BodyConfiguration body{};
		body.Type = bodytype;
		body.BodyConf.UseFixedRotation(true)
			.UseLocation({ position.x / ppu, position.y / ppu });

		switch (bodytype)
		{
			case physics::BodyTypes::STATIC:
			case physics::BodyTypes::HYBRID:
				body.BodyConf.UseType(playrho::BodyType::Static);
				break;
			case physics::BodyTypes::KINEMATIC:
				body.BodyConf.UseType(playrho::BodyType::Kinematic);
				break;
			case physics::BodyTypes::DYNAMIC:
				body.BodyConf.UseType(playrho::BodyType::Dynamic);
				break;
		}

		return body;
	}

	static physics::BodyConfiguration createPhysicsBodyConfiguration(SceneObjectPtr so, physics::BodyTypes bodytype, uint8_t ppu)
	{
		return createPhysicsBodyConfiguration(so.get(), bodytype, ppu);
	}

	static void loadPhysics(scene::ScenePtr& scene, SceneObjectPtr& so, const pugi::xml_node& physics)
	{
		if (physics.empty())
			return;

		const auto& scriptclass = so->GetClass()->GetName();
		const auto& id = so->ID;

		physics::BodyTypes bodytype{ physics::BodyTypes::KINEMATIC };
		auto ppu = scene->Physics.GetPixelsPerUnit();

		std::string type = physics.attribute("type").as_string();
		if (boost::iequals(type, "static"))
			bodytype = physics::BodyTypes::STATIC;
		else if (boost::iequals(type, "kinematic"))
			bodytype = physics::BodyTypes::KINEMATIC;
		else if (boost::iequals(type, "dynamic"))
			bodytype = physics::BodyTypes::DYNAMIC;
		else if (boost::iequals(type, "hybrid"))
			bodytype = physics::BodyTypes::HYBRID;

		// Create the body config
		auto body = helper::createPhysicsBodyConfiguration(so, bodytype, ppu);

		// Construct shapes.
		for (auto& shape : physics.children())
		{
			std::string_view name{ shape.name() };
			if (name == "box")
			{
				Vector2df v_topleft{ 0, 0 };
				Vector2df v_bottomright{ 0, 0 };

				auto center = shape.attribute("center");
				auto radius = shape.attribute("radius");
				auto left = shape.attribute("left");
				auto right = shape.attribute("right");
				auto top = shape.attribute("top");
				auto bottom = shape.attribute("bottom");
				auto topleft = shape.attribute("topleft");
				auto bottomright = shape.attribute("bottomright");
				auto dimension = shape.attribute("dimension");

				if (!center.empty() || !radius.empty())
				{
					auto centers = helper::string_split_to_numbers<float>(center.as_string());
					if (centers.size() < 2)
					{
						log::PrintLine("!! Collision box requires two values for center, skipping shape. ({}){}", scriptclass, id);
						continue;
					}

					auto radii = helper::string_split_to_numbers<float>(radius.as_string());
					if (radii.empty())
					{
						log::PrintLine("!! Collision box requires at least one value for radius, skipping shape. ({}){}", scriptclass, id);
						continue;
					}

					if (radii.size() == 1)
						radii.push_back(radii[0]);

					v_topleft.x = centers[0] - radii[0];
					v_topleft.y = centers[1] - radii[1];
					v_bottomright.x = centers[0] + radii[0];
					v_bottomright.y = centers[1] + radii[1];
				}
				else
				{
					if (!left.empty()) v_topleft.x = left.as_float();
					if (!right.empty()) v_bottomright.x = right.as_float();
					if (!top.empty()) v_topleft.y = top.as_float();
					if (!bottom.empty()) v_bottomright.y = bottom.as_float();
					if (!topleft.empty())
					{
						auto toplefts = helper::string_split_to_numbers<float>(topleft.as_string());
						if (toplefts.size() >= 2)
						{
							v_topleft.x = toplefts[0];
							v_topleft.y = toplefts[1];
						}
					}
					if (!bottomright.empty())
					{
						auto bottomrights = helper::string_split_to_numbers<float>(bottomright.as_string());
						if (bottomrights.size() >= 2)
						{
							v_bottomright.x = bottomrights[0];
							v_bottomright.y = bottomrights[1];
						}
					}
					if (!dimension.empty())
					{
						auto dimensions = helper::string_split_to_numbers<float>(dimension.as_string());
						if (dimensions.size() >= 2)
						{
							if (v_bottomright.x != 0.f)
								v_topleft.x = v_bottomright.x - dimensions[0];
							else v_bottomright.x = v_topleft.x + dimensions[0];

							if (v_bottomright.y != 0.f)
								v_topleft.y = v_bottomright.y - dimensions[1];
							else v_bottomright.y = v_topleft.y + dimensions[1];
						}
					}
				}

				// Add to the physics engine.
				playrho::Length hx = (v_bottomright.x - v_topleft.x) / 2.f;
				playrho::Length hy = (v_bottomright.y - v_topleft.y) / 2.f;
				if (hx != 0.0f && hy != 0.0f)
				{
					body.Shapes.emplace_back(playrho::d2::Shape{ playrho::d2::PolygonShapeConf{}.SetAsBox(hx, hy, { v_topleft.x + hx, v_topleft.y + hy }, 0) });;
				}
			}
			else if (name == "circle")
			{
				auto center = shape.attribute("center");
				auto radius = shape.attribute("radius");

				Vector2df v_center{ 0.f, 0.f };
				float v_radius = 0.f;

				if (!center.empty() && !radius.empty())
				{
					auto centers = helper::string_split_to_numbers<float>(center.as_string());
					if (centers.size() < 2)
					{
						log::PrintLine("!! Collision circle requires two values for center, skipping shape. ({}){}", scriptclass, id);
						continue;
					}

					v_center.x = centers[0];
					v_center.y = centers[1];
					v_radius = radius.as_float();
				}

				// Add to the physics engine.
				if (v_radius != 0.0f)
				{
					body.Shapes.emplace_back(playrho::d2::Shape{ playrho::d2::DiskShapeConf{ v_radius }.UseLocation({ v_center.x, v_center.y }) });
				}
			}
		}

		so->SetPhysicsBody(body);
	}

	static void addToScene(server::Server& server, scene::ScenePtr& scene, SceneObjectPtr& so)
	{
		log::PrintLine("[LOAD] Loaded scene object {} ({}).", so->ID, so->GetClass()->GetName());
		server.AddSceneObject(so);
		server.SwitchSceneObjectScene(so, scene);
	}

	static bool loadTileMap(server::Server& server, scene::ScenePtr& scene, SceneObjectPtr& so, const pugi::xml_node& object)
	{
		auto tiled_so = std::dynamic_pointer_cast<TiledSceneObject>(so);
		const auto& tileset = object.child("tileset");
		const auto& tiledata = object.child("tiledata");

		// TODO: Throw error.
		if (tileset.empty() || tiledata.empty())
		{
			log::PrintLine("** TileMap object requires a <tileset> or <tiledata> node, skipping. ({}){}", so->GetClass()->GetName(), so->ID);
			return false;
		}

		// Tileset
		{
			std::string file = tileset.attribute("file").as_string();
			tiled_so->Tileset = server.GetTileset(file);
		}

		// Tiles
		{
			int width = tiledata.attribute("width").as_int();
			int height = tiledata.attribute("height").as_int();
			std::string format = tiledata.attribute("format").as_string();

			tiled_so->TileData.clear();
			tiled_so->Dimension.x = width;
			tiled_so->Dimension.y = height;

			bool tdrp_format = true;
			if (boost::iequals(format, "graalnw"))
				tdrp_format = false;

			if (tdrp_format)
			{
				std::vector<uint8_t> tiles = base64::from(tiledata.text().get());
				if (tiles.size() == (static_cast<size_t>(width) * height * 2))
				{
					for (size_t i = 0; i < tiles.size(); i += 2)
					{
						tiled_so->TileData.push_back(tiles[i]);
						tiled_so->TileData.push_back(tiles[i + 1]);
					}
				}
			}
			else
			{
				const std::string base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
				std::string tiles{ tiledata.text().get() };
				boost::remove_erase_if(tiles, boost::is_space());

				if (tiles.size() == (static_cast<size_t>(width) * height * 2))
				{
					tiled_so->TileData.reserve(static_cast<size_t>(width) * height * 2);
					for (size_t i = 0; i < tiles.size(); i += 2)
					{
						uint16_t tile = (uint16_t)((base64.find(tiles[i]) << 6) + base64.find(tiles[i + 1]));
						uint8_t x = ((tile / 512) * 16) + tile % 16;
						uint8_t y = (tile % 512) / 16;

						tiled_so->TileData.push_back(x);
						tiled_so->TileData.push_back(y);
					}
				}
			}
		}

		return true;
	}

	static bool loadTMX(server::Server& server, scene::ScenePtr& scene, SceneObjectPtr& so, const std::string& file)
	{
		auto tmx_so = std::dynamic_pointer_cast<TMXSceneObject>(so);
		if (tmx_so == nullptr)
			return false;

		// Load map.
		auto tmx = std::make_shared<tmx::Map>();
		if (!tmx->load(file))
		{
			log::PrintLine("** ERROR: Failed to load .tmx file. ({}){}", so->GetClass()->GetName(), so->ID);
			return false;
		}

		// Load tilesets.
		std::vector<std::pair<scene::TilesetGID, scene::TilesetPtr>> tilesets;
		for (const auto& tileset : tmx->getTilesets())
		{
			auto ts = std::make_shared<scene::Tileset>();
			ts->File = tileset.getImagePath();
			ts->TileDimensions = { tileset.getTileSize().x, tileset.getTileSize().y };
			ts->TileCount = tileset.getTileCount();
			ts->Columns = tileset.getColumnCount();
			ts->Rows = ts->TileCount / ts->Columns;
			ts->Margin = static_cast<uint8_t>(tileset.getMargin());
			ts->Spacing = static_cast<uint8_t>(tileset.getSpacing());

			scene::TilesetGID gid
			{
				.FirstGID = tileset.getFirstGID(),
				.LastGID = tileset.getLastGID(),
			};

			auto kvp = std::make_pair(std::move(gid), ts);
			tilesets.emplace_back(std::move(kvp));
		}

		// Add the tilesets to the server.
		for (const auto& [gid, tileset] : tilesets)
		{
			filesystem::path name{ tileset->File };
			server.AddTileset(name.filename().string(), tileset);
		}

		// Look through all of our layers.
		std::shared_ptr<TMXSceneObject> current_so = tmx_so;
		const auto& layers = tmx->getLayers();
		for (auto layer_num = 0; layer_num < layers.size(); ++layer_num)
		{
			const auto& layer = layers.at(layer_num);

			// Only look at tile layers.
			if (layer->getType() != tmx::Layer::Type::Tile)
				continue;

			// Set scene object properties.
			current_so->Properties[Property::Z] = static_cast<int64_t>(layer_num);
			current_so->Layer = layer_num;

			// Load the map into the scene object.
			if (!current_so->LoadMap(tmx, tilesets))
				return false;

			log::PrintLine("TMX [{}] Loaded Map {}, layer {}.", so->ID, file, layer_num);

			// Load all the chunk collisions.
			const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();
			const auto& chunks = tilelayer.getChunks();
			for (uint32_t chunk_num = 0; chunk_num < chunks.size(); ++chunk_num)
				current_so->LoadChunkCollision(chunk_num, scene);

			// Add our scene object to the scene.
			auto current_so_base = std::dynamic_pointer_cast<SceneObject>(current_so);
			helper::addToScene(server, scene, current_so_base);

			// Assemble a new scene object for the next layer.
			if (layer_num < layers.size() - 1)
			{
				std::shared_ptr<TMXSceneObject> old_so = current_so;
				current_so = std::make_shared<TMXSceneObject>(old_so->GetClass(), server.GetNextSceneObjectID());

				// Copy everything over.
				*current_so = *old_so;
			}
		}

		return true;
	}

	static bool loadTMX(server::Server& server, scene::ScenePtr& scene, const filesystem::path& level, SceneObjectPtr& so, const pugi::xml_node& xml)
	{
		std::string file = xml.attribute("file").as_string();
		return loadTMX(server, scene, so, (level / file).string());
	}

} // end namespace tdrp::loader

namespace tdrp
{

std::shared_ptr<tdrp::scene::Scene> Loader::CreateScene(server::Server& server, std::unordered_map<std::string, scene::ScenePtr>& scene_list, const std::string& scene_name, const filesystem::path& level)
{
	auto scene = std::make_shared<Scene>(level.filename().string());
	int32_t version = 1;

	// Get our _info.xml file.
	fs::File info{ level / "_info.xml" };
	if (!info)
		return nullptr;

	// Add scene to the list.
	scene_list.insert(std::make_pair(scene_name, scene));

	// Load our _info.xml file.
	{
		pugi::xml_document infodoc;
		infodoc.load(info);

		// Determine level version.
		auto n_level = infodoc.child("level");
		if (n_level)
			version = n_level.attribute("version").as_int(1);

		// Packet transmission.
		auto n_transmission = infodoc.child("transmission");
		if (n_transmission)
			scene->m_transmission_distance = n_transmission.attribute("distance").as_uint(2000);
	}

	// Load our chunks.
	for (auto& f : filesystem::directory_iterator(level))
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
					if (scriptclass.empty())
						scriptclass = "blank";

					// Get the type of scene object we are loading.
					std::string type = object.attribute("type").as_string("default");
					SceneObjectType sotype = SceneObjectType::DEFAULT;
					if (boost::iequals(type, "static"))
						sotype = SceneObjectType::STATIC;
					else if (boost::iequals(type, "animated"))
						sotype = SceneObjectType::ANIMATED;
					else if (boost::iequals(type, "tiled") || boost::iequals(type, "tilemap"))
						sotype = SceneObjectType::TILEMAP;
					else if (boost::iequals(type, "tmx"))
						sotype = SceneObjectType::TMX;
					else if (boost::iequals(type, "text"))
						sotype = SceneObjectType::TEXT;

					// Check if local.
					auto soid = server.GetNextSceneObjectID();
					if (object.attribute("local").as_bool(false))
						soid |= SceneObject::LocalIDFlag;

					// Create our scene object.
					auto so = server.CreateSceneObject(sotype, scriptclass, soid);

					// Check flags.
					so->ReplicateChanges = object.attribute("replicatechanges").as_bool(true);
					so->IgnoresEvents = object.attribute("ignoresevents").as_bool(false);

					// Load all properties.
					for (auto& prop : object.children("property"))
					{
						std::string name = prop.attribute("name").as_string();
						std::string type = prop.attribute("type").as_string();
						std::string value = prop.attribute("value").as_string();
						bool replicated = prop.attribute("replicated").as_bool(true);
						int32_t update_rate = prop.attribute("updaterate").as_int(0);

						auto attrtype = Attribute::TypeFromString(type);
						if (attrtype == AttributeType::INVALID)
							log::PrintLine("!! Attribute type was invalid, skipping: ({}){}.{}", scriptclass, so->ID, name);
						else
						{
							auto soprop = so->Properties.Get(name);
							soprop->SetAsType(attrtype, value);
							soprop->Replicated = replicated;
							soprop->NetworkUpdateRate = std::chrono::milliseconds{ update_rate };
						}
					}

					// Load all attributes.
					for (auto& prop : object.children("attribute"))
					{
						std::string name = prop.attribute("name").as_string();
						std::string type = prop.attribute("type").as_string();
						std::string value = prop.attribute("value").as_string();
						bool replicated = prop.attribute("replicated").as_bool(true);
						int32_t update_rate = prop.attribute("updaterate").as_int(0);

						auto attrtype = Attribute::TypeFromString(type);
						if (attrtype == AttributeType::INVALID)
							log::PrintLine("!! Attribute type was invalid, skipping: ({}){}.{}", scriptclass, so->ID, name);
						else
						{
							auto soattrib = so->Attributes.AddAttribute(name, Attribute::TypeFromString(type), value);
							soattrib->Replicated = replicated;
							soattrib->NetworkUpdateRate = std::chrono::milliseconds{ update_rate };
						}
					}

					// Load scripts.
					if (auto script = object.child("script"); !script.empty())
					{
						if (auto node = script.child("client"); !node.empty())
						{
							if (auto nodefile = node.attribute("file"); !nodefile.empty())
							{
								auto file = server.FileSystem.GetFile(fs::FileCategory::WORLD, nodefile.as_string());
								so->ClientScript.append(file->ReadAsString());
							}
							if (auto nodetext = node.text(); !nodetext.empty())
							{
								so->ClientScript.append(nodetext.as_string());
							}
						}

						if (auto node = script.child("server"); !node.empty())
						{
							if (auto nodefile = node.attribute("file"); !nodefile.empty())
							{
								auto file = server.FileSystem.GetFile(fs::FileCategory::WORLD, nodefile.as_string());
								so->ServerScript.append(file->ReadAsString());
							}
							if (auto nodetext = node.text(); !nodetext.empty())
							{
								so->ServerScript.append(nodetext.as_string());
							}
						}
					}

					// Load physics information.
					// Make sure this is done after properties are loaded.
					if (std::ranges::any_of(helper::allowPhysicsNode, [&sotype](const auto& type) { return type == sotype; }))
					{
						helper::loadPhysics(scene, so, object.child("physics"));
					}

					// Load tilemap features.
					if (sotype == SceneObjectType::TILEMAP)
					{
						helper::loadTileMap(server, scene, so, object);
					}

					// Load tmx features.
					if (sotype == SceneObjectType::TMX)
					{
						const auto& tmx = object.child("tmx");
						if (tmx.empty())
						{
							log::PrintLine("** ERROR: TMX object requires a <tmx> node, skipping sceneobject. ({}){}", scriptclass, so->ID);
							continue;
						}

						if (!helper::loadTMX(server, scene, level, so, tmx))
							continue;
					}

					// Execute the script and add it to the scene.
					if (so->GetCurrentScene().expired())
						helper::addToScene(server, scene, so);
				}
			}
		}
	}

	return scene;
}

} // end namespace tdrp::loader
