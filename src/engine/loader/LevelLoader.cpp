#include <charconv>
#include <concepts>
#include <ranges>

#include "engine/loader/Loader.h"

#include "engine/filesystem/File.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/scene/SceneObject.h"
#include "engine/scene/Scene.h"
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

	playrho::BodyID getOrCreatePhysicsBody(playrho::d2::World& world, SceneObject* so, playrho::BodyType bodytype, uint8_t ppu)
	{
		if (so->PhysicsBody.has_value())
			return so->PhysicsBody.value();

		auto position = so->GetPosition();

		playrho::d2::BodyConf config;
		config
			.UseType(bodytype)
			.UseLocation({ position.x / ppu, position.y / ppu })
			.UseFixedRotation(true);

		auto body = playrho::d2::CreateBody(world, config);
		so->PhysicsBody = body;
		return body;
	}

	playrho::BodyID getOrCreatePhysicsBody(playrho::d2::World& world, SceneObjectPtr so, playrho::BodyType bodytype, uint8_t ppu)
	{
		return getOrCreatePhysicsBody(world, so.get(), bodytype, ppu);
	}

	std::pair<int32_t, int32_t> getTilePosition(tmx::RenderOrder render_order, const tmx::Vector2i& chunk_size, uint32_t index)
	{
		std::pair<int32_t, int32_t> result;

		switch (render_order)
		{
		case tmx::RenderOrder::RightDown:
			result.first = (chunk_size.x - 1) - (index % chunk_size.x);
			result.second = index / chunk_size.y;
			break;
		case tmx::RenderOrder::RightUp:
			result.first = (chunk_size.x - 1) - (index % chunk_size.x);
			result.second = (chunk_size.y - 1) - (index / chunk_size.y);
			break;
		case tmx::RenderOrder::LeftDown:
			result.first = index % chunk_size.x;
			result.second = index / chunk_size.y;
			break;
		case tmx::RenderOrder::LeftUp:
			result.first = index % chunk_size.x;
			result.second = (chunk_size.y - 1) - (index / chunk_size.y);
			break;
		}

		return result;
	}

	void collectPolygons(const std::unique_ptr<Clipper2Lib::PolyPathD>& polypath, TPPLPolyList& polylist)
	{
		const auto& poly = polypath->Polygon();

		TPPLPoly polygon;
		polygon.Init(static_cast<long>(poly.size()));
		polygon.SetHole(polypath->IsHole());

		if (polypath->IsHole())
			polygon.SetOrientation(TPPL_ORIENTATION_CCW);
		else polygon.SetOrientation(TPPL_ORIENTATION_CW);

		for (auto i = 0; i < poly.size(); ++i)
		{
			polygon[i].x = poly[i].x;
			polygon[i].y = poly[i].y;
		}

		polylist.emplace_back(std::move(polygon));

		for (const auto& child : *polypath)
			collectPolygons(child, polylist);
	}

	void createPhysicsShape(playrho::d2::World& world, const playrho::BodyID& body, const uint8_t& ppu, const TPPLPoly& polygon)
	{
		if (polygon.IsHole()) return;

		// Convert to PlayRho vertices.
		std::vector<playrho::Length2> vertices;
		for (auto i = 0; i < polygon.GetNumPoints(); ++i)
		{
			const auto& point = polygon[i];
			vertices.emplace_back(playrho::Length2{ static_cast<float>(point.x) / ppu, static_cast<float>(point.y) / ppu });
		}

		// Set the shape config.
		playrho::d2::PolygonShapeConf conf{};
		conf.UseVertices(vertices);

		// Add the shape to the world.
		const auto& shape = playrho::d2::CreateShape(world, conf);
		playrho::d2::Attach(world, body, shape);
	}

	void loadPhysics(scene::ScenePtr& scene, SceneObjectPtr& so, const pugi::xml_node& physics)
	{
		if (physics.empty())
			return;

		const auto& scriptclass = so->GetClass()->GetName();
		const auto& id = so->ID;

		playrho::BodyType bodytype{ playrho::BodyType::Kinematic };
		auto& world = scene->Physics.GetWorld();
		auto ppu = scene->Physics.GetPixelsPerUnit();

		std::string type = physics.attribute("type").as_string();
		if (boost::iequals(type, "static"))
			bodytype = playrho::BodyType::Static;
		else if (boost::iequals(type, "kinematic"))
			bodytype = playrho::BodyType::Kinematic;
		else if (boost::iequals(type, "dynamic"))
			bodytype = playrho::BodyType::Dynamic;

		// Create the body.
		auto body = helper::getOrCreatePhysicsBody(world, so, bodytype, ppu);

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
					auto shape = playrho::d2::CreateShape(world, playrho::d2::PolygonShapeConf{ hx, hy }.Translate({ v_topleft.x + hx, v_topleft.y + hy }));
					playrho::d2::Attach(world, so->PhysicsBody.value(), shape);
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
					auto shape = playrho::d2::CreateShape(world, playrho::d2::DiskShapeConf{ v_radius }.UseLocation({ v_center.x, v_center.y }));
					playrho::d2::Attach(world, so->PhysicsBody.value(), shape);
				}
			}
		}
	}

	void addToScene(server::Server& server, scene::ScenePtr& scene, SceneObjectPtr& so)
	{
		const auto& c = so->GetClass();
		const auto& id = so->ID;

		// Handle the script.
		if (server.IsHost())
		{
			server.Script->RunScript("sceneobject_sv_" + std::to_string(id) + "_c_" + c->GetName(), c->ScriptServer, so);
			server.Script->RunScript("sceneobject_sv_" + std::to_string(id), so->ServerScript, so);
		}
		server.Script->RunScript("sceneobject_cl_" + std::to_string(id) + "_c_" + c->GetName(), c->ScriptClient, so);
		server.Script->RunScript("sceneobject_cl_" + std::to_string(id), so->ClientScript, so);

		// Add the object to the scene.
		scene->AddObject(so);
	}

	bool loadTileMap(server::Server& server, scene::ScenePtr& scene, SceneObjectPtr& so, const pugi::xml_node& object)
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
				if (tiles.size() == (width * height * 2))
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

				if (tiles.size() == (width * height * 2))
				{
					tiled_so->TileData.reserve(width * height * 2);
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

	bool loadTMX(server::Server& server, scene::ScenePtr& scene, const filesystem::path& level, SceneObjectPtr& so, const pugi::xml_node& xml)
	{
		auto tmx_so = std::dynamic_pointer_cast<TMXSceneObject>(so);
		if (tmx_so == nullptr)
			return false;

		// Map
		{
			std::string file = xml.attribute("file").as_string();

			tmx_so->TmxMap = std::make_shared<tmx::Map>();
			if (!tmx_so->TmxMap->load((level / file).string()))
			{
				log::PrintLine("** ERROR: Failed to load .tmx file. ({}){}", so->GetClass()->GetName(), so->ID);
				return false;
			}
		}

		auto& tmx = tmx_so->TmxMap;
		const auto& tilesets = tmx->getTilesets();
		const auto& tile_count = tmx->getTileCount();
		const auto& tile_size = tmx->getTileSize();

		// Search all the tilesets to find the details on a tile.
		auto find_tile_in_tileset = [&tilesets](const tmx::TileLayer::Tile& layer_tile) -> const tmx::Tileset::Tile*
		{
			for (const auto& tileset : tilesets)
			{
				const auto* tile = tileset.getTile(layer_tile.ID);
				return tile;
			}
			return nullptr;
		};

		auto& world = scene->Physics.GetWorld();
		auto ppu = scene->Physics.GetPixelsPerUnit();
		auto current_so = std::dynamic_pointer_cast<TMXSceneObject>(so);

		// Look through all of our layers.
		const auto& layers = tmx->getLayers();
		for (auto layer_num = 0; layer_num < layers.size(); ++layer_num)
		{
			const auto& layer = layers.at(layer_num);

			// Only look at tile layers.
			if (layer->getType() != tmx::Layer::Type::Tile)
				continue;

			// Go through all of the chunks.
			const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();
			const auto& chunks = tilelayer.getChunks();
			for (auto chunk_num = 0; chunk_num < chunks.size(); ++chunk_num)
			{
				const auto& chunk = chunks.at(chunk_num);

				// Save the chunk properties.
				current_so->SetPosition({ static_cast<float>(chunk.position.x) * ppu, static_cast<float>(chunk.position.y) * ppu });
				current_so->Properties[Property::Z] = static_cast<int64_t>(layer_num);
				current_so->Layer = layer_num;
				current_so->Chunk = chunk_num;
				current_so->ChunkSize = { static_cast<float>(chunk.size.x) * ppu, static_cast<float>(chunk.size.y) * ppu };

				Clipper2Lib::PathsD collision_polys;

				// Look through all the tiles in the chunk.
				auto tile_count = chunk.size.x * chunk.size.y;
				for (int i = 0; i < tile_count; ++i)
				{
					// Find our tileset tile entry.
					// If we have no entry, we have no collision so skip.
					const auto& layer_tile = chunk.tiles[i];
					const auto* tile = find_tile_in_tileset(layer_tile);
					if (tile == nullptr)
						continue;

					// Calculate the tile position.
					auto [x, y] = helper::getTilePosition(tmx->getRenderOrder(), chunk.size, i);

					// Check if we have a polygon collision specified.
					auto has_points = std::ranges::find_if(tile->objectGroup.getObjects(), [](const tmx::Object& item) { return item.getPoints().size() != 0; });
					if (has_points != std::ranges::end(tile->objectGroup.getObjects()))
					{
						Clipper2Lib::PathD poly;
						const auto& points = has_points->getPoints();
						for (const auto& point : points)
							poly.emplace_back(Clipper2Lib::PointD{ (x * tile_size.x) + point.x, (y * tile_size.y) + point.y });

						collision_polys.push_back(std::move(poly));
					}
					// Otherwise, we may have a box collision.
					else if (tile->objectGroup.getObjects().size() == 1)
					{
						const auto& object = tile->objectGroup.getObjects().at(0);
						const auto& aabb = object.getAABB();
						if (aabb.width != 0)
						{
							Clipper2Lib::PathD poly;
							float px = x * tile_size.x + aabb.left;
							float py = y * tile_size.y + aabb.top;

							poly.emplace_back(Clipper2Lib::PointD{ px, py });
							poly.emplace_back(Clipper2Lib::PointD{ px + aabb.width, py });
							poly.emplace_back(Clipper2Lib::PointD{ px + aabb.width, py + aabb.height });
							poly.emplace_back(Clipper2Lib::PointD{ px, py + aabb.height });
							collision_polys.push_back(std::move(poly));
						}
					}
				}

				// If we have collision data, process it.
				if (!collision_polys.empty())
				{
					Clipper2Lib::ClipperD clip;
					clip.PreserveCollinear = false;
					clip.AddSubject(collision_polys);

					// Union all the polygons.
					Clipper2Lib::PolyTreeD polytree;
					clip.Execute(Clipper2Lib::ClipType::Union, Clipper2Lib::FillRule::NonZero, polytree);

					// Construct the collision polys.
					TPPLPolyList polygons;
					for (const auto& polychild : polytree)
						helper::collectPolygons(polychild, polygons);

					// Partition the polygons into convex shapes.
					TPPLPartition partition;
					TPPLPolyList partitioned_polys;
					partition.ConvexPartition_HM(&polygons, &partitioned_polys);

					// Create physics shapes on the vertices.
					auto body = helper::getOrCreatePhysicsBody(world, current_so, playrho::BodyType::Static, ppu);
					for (const auto& polychild : partitioned_polys)
						helper::createPhysicsShape(world, body, ppu, polychild);
				}

				// Add our scene object to the scene.
				auto current_so_base = std::dynamic_pointer_cast<SceneObject>(current_so);
				helper::addToScene(server, scene, current_so_base);
				//log::PrintLine(":: Added TMX ({}), layer {}, chunk {}.", current_so->ID, current_so->Layer, current_so->Chunk);

				// Assemble a new scene object for the next chunk.
				if (layer_num != layers.size() && chunk_num != chunks.size())
				{
					auto old_so = current_so;
					current_so = std::make_shared<TMXSceneObject>(old_so->GetClass(), server.GetNextSceneObjectID());

					// Copy everything over.
					*current_so = *old_so;
				}
				else current_so = nullptr;
			}
		}

		return true;
	}

} // end namespace tdrp::loader

namespace tdrp
{

std::shared_ptr<tdrp::scene::Scene> Loader::CreateScene(server::Server& server, const filesystem::path& level)
{
	auto scene = std::make_shared<Scene>(level.filename().string());
	int32_t version = 1;

	// Get our _info.xml file.
	fs::File info{ level / "_info.xml" };
	if (!info)
		return nullptr;

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
					auto c = server.GetObjectClass(scriptclass);
					if (c == nullptr)
						c = server.GetObjectClass("blank");

					uint32_t id = server.GetNextSceneObjectID();

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

					// Create our scene object.
					std::shared_ptr<SceneObject> so = nullptr;
					switch (sotype)
					{
						case SceneObjectType::STATIC: so = std::make_shared<StaticSceneObject>(c, id); break;
						case SceneObjectType::ANIMATED: so = std::make_shared<AnimatedSceneObject>(c, id); break;
						case SceneObjectType::TILEMAP: so = std::make_shared<TiledSceneObject>(c, id); break;
						case SceneObjectType::TMX: so = std::make_shared<TMXSceneObject>(c, id); break;
						case SceneObjectType::TEXT: so = std::make_shared<TextSceneObject>(c, id); break;
						default: so = std::make_shared<SceneObject>(c, id); break;
					}

					// Check if it is a non-replicated scene object.
					so->Replicated = object.attribute("replicated").as_bool(true);

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
							log::PrintLine("!! Attribute type was invalid, skipping: ({}){}.{}", scriptclass, id, name);
						else
						{
							auto soprop = so->Properties.Get(name);
							soprop->SetAsType(attrtype, value);
							soprop->Replicated = replicated;
							soprop->NetworkUpdate.UpdateRate = std::chrono::milliseconds{ update_rate };
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
							log::PrintLine("!! Attribute type was invalid, skipping: ({}){}.{}", scriptclass, id, name);
						else
						{
							auto soattrib = so->Attributes.AddAttribute(name, Attribute::TypeFromString(type), value);
							soattrib->Replicated = replicated;
							soattrib->NetworkUpdate.UpdateRate = std::chrono::milliseconds{ update_rate };
						}
					}

					// Load scripts.
					if (auto script = object.child("script"); !script.empty())
					{
						if (auto node = script.child("client"); !node.empty())
						{
							if (auto nodefile = node.attribute("file"); !nodefile.empty())
							{
								auto file = server.FileSystem.GetFile(nodefile.as_string());
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
								auto file = server.FileSystem.GetFile(nodefile.as_string());
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
							log::PrintLine("** ERROR: TMX object requires a <tmx> node, skipping sceneobject. ({}){}", scriptclass, id);
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
