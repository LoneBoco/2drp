// #include <Box2D.h>
#include <deque>
#include <set>
#include <sstream>

#include <PlayRho/Dynamics/Body.hpp>
#include <clipper2/clipper.h>
#include <polypartition.h>

#include "engine/common.h"
#include "engine/server/Player.h"
#include "engine/scene/Scene.h"
#include "engine/filesystem/Log.h"

#include "SceneObject.h"


namespace tdrp::helper
{
	static std::pair<int32_t, int32_t> getTilePosition(tmx::RenderOrder render_order, const tmx::Vector2i& chunk_size, uint32_t index)
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

	static void collectPolygons(const std::unique_ptr<Clipper2Lib::PolyPathD>& polypath, TPPLPolyList& polylist)
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

	static std::optional<playrho::d2::Shape> createPhysicsShape(physics::BodyConfiguration& body, tmx::Vector2i position, const uint8_t& ppu, const TPPLPoly& polygon)
	{
		if (polygon.IsHole()) return std::nullopt;

		// Convert to PlayRho vertices.
		std::vector<playrho::Length2> vertices;
		for (auto i = 0; i < polygon.GetNumPoints(); ++i)
		{
			const auto& point = polygon[i];
			vertices.emplace_back(playrho::Length2{ position.x + static_cast<float>(point.x) / ppu, position.y + static_cast<float>(point.y) / ppu });
		}

		// Set the shape config.
		playrho::d2::PolygonShapeConf conf{};
		conf.UseVertices(vertices);

		// Save the shape.
		body.Shapes.emplace_back(playrho::d2::Shape{ conf });
		return std::make_optional(body.Shapes.back());
	}

} // end namespace tdrp::helper

namespace tdrp
{

SceneObject::SceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id)
	: ID{ id }, Visible{ true }, m_object_class{ c }
{
	if (c)
	{
		Attributes = c->Attributes;
	}
}

SceneObject::~SceneObject()
{
}

SceneObject& SceneObject::operator=(const SceneObject& other)
{
	Name = other.Name + std::to_string(static_cast<uint16_t>(reinterpret_cast<uintptr_t>(this) & 0x0000'0000'FFFF'FFFF));
	ClientScript = other.ClientScript;
	ServerScript = other.ServerScript;
	Attributes = other.Attributes;
	Properties = other.Properties;
	Visible = other.Visible;
	ReplicateChanges = other.ReplicateChanges;
	IgnoresEvents = other.IgnoresEvents;
	Initialized = false;
	return *this;
}

Vector2df SceneObject::GetPosition() const
{
	if (m_attached_to.expired())
	{
		return Vector2df{
			Properties[Property::X].GetAs<float>(),
			Properties[Property::Y].GetAs<float>()
		};
	}
	else
	{
		auto attached = m_attached_to.lock();
		if (!attached)
			return Vector2df{ Properties[Property::X].GetAs<float>(), Properties[Property::Y].GetAs<float>() };
		else
		{
			Vector2df pos{ Properties[Property::X].GetAs<float>(), Properties[Property::Y].GetAs<float>() };
			return attached->GetPosition() + pos;
		}
	}
}

void SceneObject::SetPosition(const Vector2df& position)
{
 	Properties[Property::X] = position.x;
	Properties[Property::Y] = position.y;

	// If we have no bodies or we are attached to another scene object, don't bother updating anything.
	if (!HasPhysicsBody() || !m_attached_to.expired())
		return;

	if (auto scene = m_current_scene.lock(); scene)
	{
		auto& world = scene->Physics.GetWorld();
		auto ppu = scene->Physics.GetPixelsPerUnit();
		auto attached_to = m_attached_to.lock();

		if (!attached_to)
			playrho::d2::SetLocation(world, m_physics_body.value(), {position.x / ppu, position.y / ppu});
		else
		{
			Vector2df attached_pos{ attached_to->GetPosition() + position };
			playrho::d2::SetLocation(world, m_physics_body.value(), {attached_pos.x / ppu, attached_pos.y / ppu});
		}
	}
}

int64_t SceneObject::GetDepth() const
{
	return Properties[Property::Z].GetAs<int64_t>();
}

void SceneObject::SetDepth(const int64_t& z)
{
	Properties[Property::Z] = z;
}

float SceneObject::GetAngle() const
{
	return Properties[Property::ANGLE].GetAs<float>();
}

void SceneObject::SetAngle(const float& angle)
{
	Properties[Property::ANGLE] = angle;

	if (!HasPhysicsBody())
		return;

	if (auto scene = m_current_scene.lock(); scene)
	{
		auto& world = scene->Physics.GetWorld();
		playrho::d2::SetAngle(world, m_physics_body.value(), angle);
	}
}

Vector2df SceneObject::GetScale() const
{
	return Vector2df{
		Properties[Property::SCALE_X].GetAs<float>(),
		Properties[Property::SCALE_Y].GetAs<float>()
	};
}

void SceneObject::SetScale(const Vector2df& scale)
{
	Properties[Property::SCALE_X] = scale.x;
	Properties[Property::SCALE_Y] = scale.y;

	// TODO: Physics scale.
	/*
	if (PhysicsBody.has_value())
	{
		if (auto scene = m_current_scene.lock(); scene)
		{
			auto& world = scene->Physics.GetWorld();
		}
	}
	*/

/*
	// Scale the physics objects.
	if (changed)
	{
		std::vector<b2Fixture*> oldFixtures;
		std::vector<b2FixtureDef> newFixtures;
		float x = scale.X;
		float y = scale.Y;

		// Scale relative to the old scale.
		x /= old_x;
		y /= old_y;

		// Iterate through every fixture.
		b2Fixture* fixture = Physics.Body->GetFixtureList();
		while (true)
		{
			// Box2D breaks if you just adjust the scale of the vertices.
			// Deleting the old fixture and creating a new one will properly fix the physics engine.
			// Push old fixture so we can delete it later.
			oldFixtures.push_back(fixture);

			// Create a new fixture duplicating the properties of the old one.
			b2FixtureDef newFixture;
			newFixture.density = fixture->GetDensity();
			newFixture.filter = fixture->GetFilterData();
			newFixture.friction = fixture->GetFriction();
			newFixture.isSensor = fixture->IsSensor();
			newFixture.restitution = fixture->GetRestitution();
			newFixture.shape = fixture->GetShape();
			newFixture.userData = fixture->GetUserData();
			newFixtures.push_back(newFixture);

			// Scale the shape!
			b2Shape* shape = fixture->GetShape();
			switch (shape->GetType())
			{
				case b2Shape::e_chain:
				{
					b2ChainShape* chain = reinterpret_cast<b2ChainShape*>(shape);
					for (int i = 0; i < chain->m_count; ++i)
					{
						b2Vec2& v = chain->m_vertices[i];
						v.x *= x;
						v.y *= y;
					}
					chain->m_nextVertex.x *= x;
					chain->m_nextVertex.y *= y;
					chain->m_prevVertex.x *= x;
					chain->m_prevVertex.y *= y;
					break;
				}
				case b2Shape::e_circle:
				{
					b2CircleShape* circle = reinterpret_cast<b2CircleShape*>(shape);
					circle->m_radius *= x;
					circle->m_p.x *= x;
					circle->m_p.y *= y;
					break;
				}
				case b2Shape::e_edge:
				{
					b2EdgeShape* edge = reinterpret_cast<b2EdgeShape*>(shape);
					edge->m_vertex1 = b2Vec2(edge->m_vertex1.x * x, edge->m_vertex1.y * y);
					edge->m_vertex2 = b2Vec2(edge->m_vertex2.x * x, edge->m_vertex2.y * y);
					if (edge->m_hasVertex0) edge->m_vertex0 = b2Vec2(edge->m_vertex0.x * x, edge->m_vertex0.y * y);
					if (edge->m_hasVertex3) edge->m_vertex3 = b2Vec2(edge->m_vertex3.x * x, edge->m_vertex3.y * y);
					break;
				}
				case b2Shape::e_polygon:
				{
					b2PolygonShape* poly = reinterpret_cast<b2PolygonShape*>(shape);
					for (int i = 0; i < poly->m_count; ++i)
					{
						poly->m_vertices[i].x *= x;
						poly->m_vertices[i].y *= y;
					}
					poly->m_centroid.x *= x;
					poly->m_centroid.y *= y;
					break;
				}
			}

			// Get the next fixture.
			fixture = fixture->GetNext();
			if (fixture == nullptr)
				break;
		}

		// Add all the new fixtures.
		for (auto i = newFixtures.begin(); i != newFixtures.end(); ++i)
			Physics.Body->CreateFixture(&(*i));
		
		// Delete all the old fixtures.
		for (auto i = oldFixtures.begin(); i != oldFixtures.end(); ++i)
			Physics.Body->DestroyFixture(*i);
	}
*/
}

Vector2df SceneObject::GetVelocity() const
{
	return Vector2df{
		Properties[Property::VELOCITY_X].GetAs<float>(),
		Properties[Property::VELOCITY_Y].GetAs<float>()
	};
}

void SceneObject::SetVelocity(const Vector2df& velocity)
{
	Properties[Property::VELOCITY_X] = velocity.x;
	Properties[Property::VELOCITY_Y] = velocity.y;

	if (!HasPhysicsBody())
		return;

	if (auto scene = m_current_scene.lock(); scene)
	{
		auto& world = scene->Physics.GetWorld();
		SetHybridBodyMoving(velocity.x != 0.0f || velocity.y != 0.0f);
		playrho::d2::SetVelocity(world, m_physics_body.value(), {velocity.x, velocity.y});
	}
}

Vector2df SceneObject::GetAcceleration() const
{
	return Vector2df{
		Properties[Property::ACCELERATION_X].GetAs<float>(),
		Properties[Property::ACCELERATION_Y].GetAs<float>()
	};
}

void SceneObject::SetAcceleration(const Vector2df& acceleration)
{
	Properties[Property::ACCELERATION_X] = acceleration.x;
	Properties[Property::ACCELERATION_Y] = acceleration.y;

	if (!HasPhysicsBody())
		return;

	if (auto scene = m_current_scene.lock(); scene)
	{
		auto& world = scene->Physics.GetWorld();
		playrho::d2::SetAcceleration(world, m_physics_body.value(), {acceleration.x, acceleration.y});
	}
}

float SceneObject::GetVelocityAngle() const
{
	return Properties[Property::VELOCITY_ANGLE].GetAs<float>();
}

void SceneObject::SetVelocityAngle(const float& velocity)
{
	Properties[Property::VELOCITY_ANGLE] = velocity;

	if (!HasPhysicsBody())
		return;

	if (auto scene = m_current_scene.lock(); scene)
	{
		auto& world = scene->Physics.GetWorld();
		playrho::d2::SetVelocity(world, m_physics_body.value(), velocity);
	}
}

float SceneObject::GetAccelerationAngle() const
{
	return Properties[Property::ACCELERATION_ANGLE].GetAs<float>();
}

void SceneObject::SetAccelerationAngle(const float& acceleration)
{
	Properties[Property::ACCELERATION_ANGLE] = acceleration;

	if (!HasPhysicsBody())
		return;

	if (auto scene = m_current_scene.lock(); scene)
	{
		auto& world = scene->Physics.GetWorld();
		playrho::d2::SetAcceleration(world, m_physics_body.value(), acceleration);
	}
}

int64_t SceneObject::GetDirection() const
{
	return Properties[Property::DIRECTION].GetAs<int64_t>();
}

void SceneObject::SetDirection(const int64_t& dir)
{
	Properties[Property::DIRECTION] = dir;
}

std::string SceneObject::GetImage() const
{
	return Properties.Get(Property::IMAGE)->GetAs<std::string>();
}

void SceneObject::SetImage(const std::string& image)
{
	Properties[Property::IMAGE] = image;
}

std::string SceneObject::GetEntity() const
{
	return Properties.Get(Property::ENTITY)->GetAs<std::string>();
}

void SceneObject::SetEntity(const std::string& image)
{
	Properties[Property::ENTITY] = image;
}

std::string SceneObject::GetAnimation() const
{
	return Properties.Get(Property::ANIMATION)->GetAs<std::string>();
}

void SceneObject::SetAnimation(const std::string& image)
{
	Properties[Property::ANIMATION] = image;
}

std::string SceneObject::GetText() const
{
	return Properties.Get(Property::TEXT)->GetAs<std::string>();
}

void SceneObject::SetText(const std::string& text)
{
	Properties[Property::TEXT] = text;
}

///////////////////////////////////////////////////////////////////////////////

Rectf SceneObject::GetBounds() const noexcept
{
	auto bbox = this->RetrieveFromProvider("BoundingBox");
	if (bbox.has_value() && bbox.type() == typeid(Rectf))
	{
		auto r = std::any_cast<Rectf>(bbox);
		return r;
	}

	auto size = this->RetrieveFromProvider("Size");
	if (size.has_value() && size.type() == typeid(Vector2df))
	{
		auto s = std::any_cast<Vector2df>(size);
		return Rectf(GetPosition(), s);
	}
	return Rectf(GetPosition(), Vector2df(0.0f));
}

bool SceneObject::IsOwnedBy(PlayerID id) const noexcept
{
	return id == m_owning_player;
}

bool SceneObject::IsOwnedBy(std::shared_ptr<server::Player> player) const noexcept
{
	return player != nullptr && IsOwnedBy(player->GetPlayerId());
}

void SceneObject::SetOwningPlayer(PlayerID player)
{
	m_owning_player = player;
}

void SceneObject::SetOwningPlayer(std::shared_ptr<server::Player> player)
{
	m_owning_player = player->GetPlayerId();
}

bool SceneObject::SetCurrentScene(std::shared_ptr<scene::Scene> scene)
{
	if (!IsLocal() || m_current_scene.expired())
	{
		m_current_scene = scene;
		return true;
	}

	return false;
}

void SceneObject::AttachTo(std::shared_ptr<SceneObject> other)
{
	auto old = m_attached_to.lock();

	if (old && old == other)
		return;

	m_attached_to = other;
	other->m_attached.push_back(weak_from_this());

	// Enable or disable the bodies depending on the attach status.
	if (auto scene = m_current_scene.lock(); scene && HasPhysicsBody())
	{
		auto& world = scene->Physics.GetWorld();
		playrho::d2::SetEnabled(world, m_physics_body.value(), other != nullptr);
	}

	// If we are detaching, set our position to our current location.
	// If we are attaching, set our position offset to 0,0.
	if (other == nullptr)
	{
		log::PrintLine(log::game, ":: Detaching scene object {}.", ID);
		SetPosition(other->GetPosition() + GetPosition());
	}
	else
	{
		log::PrintLine(log::game, ":: Attaching scene object {} to {}.", ID, other->ID);
		SetPosition({ 0, 0 });
	}

	OnAttached.RunAll(m_attached_to, old);
}

SceneObjectID SceneObject::GetAttachedId() const noexcept
{
	if (m_attached_to.expired()) return 0;
	auto attached = m_attached_to.lock();
	if (!attached) return 0;
	return attached->ID;
}

void SceneObject::addPhysicsBodyToScene(std::shared_ptr<scene::Scene>& scene) noexcept
{
	if (HasPhysicsBody())
		return;
	if (!m_physics_body_config.has_value() || m_physics_body_config.value().Shapes.empty())
		return;

	auto& world = scene->Physics.GetWorld();
	auto ppu = scene->Physics.GetPixelsPerUnit();
	auto position = GetPosition();

	// Update the config before we add it.
	physics::BodyConfiguration config{ m_physics_body_config.value()};
	config.BodyConf.UseLocation({ position.x / ppu, position.y / ppu });
	if (!m_attached_to.expired())
		config.BodyConf.enabled = false;

	// Set the body type.
	bool has_shadow = false;
	switch (config.Type)
	{
		case physics::BodyTypes::STATIC:
		case physics::BodyTypes::HYBRID:
			config.BodyConf.UseType(playrho::BodyType::Static);
			break;
		case physics::BodyTypes::KINEMATIC:
			config.BodyConf.UseType(playrho::BodyType::Kinematic);
			break;
		case physics::BodyTypes::DYNAMIC:
			config.BodyConf.UseType(playrho::BodyType::Dynamic);
			break;
	}

	// Create the body.
	auto body = playrho::d2::CreateBody(world, config.BodyConf);
	m_physics_body = body;
	scene->Physics.AddBodyToSceneObject(body, shared_from_this());

	// Add the shapes to the body.
	for (playrho::d2::Shape shape : config.Shapes)
	{
		playrho::d2::FixtureConf fixture_conf{};
		fixture_conf.UseBody(body).UseShape(shape);
		fixture_conf.filter.categoryBits = physics::category_default;

		playrho::d2::CreateFixture(world, fixture_conf);
	}
}

void SceneObject::SetPhysicsBody(const physics::BodyConfiguration& body_config) noexcept
{
	m_physics_body_config = body_config;

	auto scene = m_current_scene.lock();
	if (!scene) return;

	addPhysicsBodyToScene(scene);
}

void SceneObject::SetHybridBodyMoving(bool is_moving) noexcept
{
	auto scene = m_current_scene.lock();
	if (!scene) return;

	SetHybridBodyMoving(scene->Physics.GetWorld(), is_moving);
}

void SceneObject::SetHybridBodyMoving(playrho::d2::World& world, bool is_moving) noexcept
{
	if (!HasPhysicsBody() || !m_physics_body_config.has_value())
		return;
	if (m_physics_body_config.value().Type != physics::BodyTypes::HYBRID)
		return;

	auto& body = m_physics_body.value();
	auto current_type = playrho::d2::GetType(world, body);
	
	// If we are already in the correct state, don't change anything.
	if (current_type == playrho::BodyType::Dynamic && is_moving)
		return;
	if (current_type == playrho::BodyType::Static && !is_moving)
		return;

	playrho::d2::SetType(world, body, is_moving ? playrho::BodyType::Dynamic : playrho::BodyType::Static, true);

	// Just for debug drawing purposes.
	for (const auto& fixture : playrho::d2::GetFixtures(world, body))
	{
		auto filter = playrho::d2::GetFilterData(world, fixture);
		filter.categoryBits = is_moving ? physics::category_hybrid : physics::category_default;
		playrho::d2::SetFilterData(world, fixture, filter);
	}
}

void SceneObject::ConstructPhysicsBodiesFromConfiguration() noexcept
{
	auto scene = m_current_scene.lock();
	if (!scene) return;

	DestroyAllPhysicsBodies();
	addPhysicsBodyToScene(scene);
}

void SceneObject::DestroyAllPhysicsBodies() noexcept
{
	if (!HasPhysicsBody())
		return;

	if (auto scene = m_current_scene.lock(); scene)
	{
		auto& world = scene->Physics.GetWorld();
		world.Destroy(m_physics_body.value());
	}

	m_physics_body.reset();
}

void SceneObject::SynchronizePhysics()
{
	if (!HasPhysicsBody())
		return;

	bool x_dirty = Properties[Property::X].Dirty;
	bool y_dirty = Properties[Property::Y].Dirty;
	if (x_dirty || y_dirty)
		SetPosition(GetPosition());

	bool angle = Properties[Property::ANGLE].Dirty;
	if (angle)
		SetAngle(GetAngle());

	bool x_velocity = Properties[Property::VELOCITY_X].Dirty;
	bool y_velocity = Properties[Property::VELOCITY_Y].Dirty;
	if (x_velocity || y_velocity)
		SetVelocity(GetVelocity());

	bool x_acceleration = Properties[Property::ACCELERATION_X].Dirty;
	bool y_acceleration = Properties[Property::ACCELERATION_Y].Dirty;
	if (x_acceleration || y_acceleration)
		SetAcceleration(GetAcceleration());

	bool angle_velocity = Properties[Property::VELOCITY_ANGLE].Dirty;
	if (angle_velocity)
		SetVelocityAngle(GetVelocityAngle());

	bool angle_acceleration = Properties[Property::ACCELERATION_ANGLE].Dirty;
	if (angle_acceleration)
		SetAccelerationAngle(GetAccelerationAngle());
}

///////////////////////////////////////////////////////////////////////////////

AnimatedSceneObject& AnimatedSceneObject::operator=(const AnimatedSceneObject& other)
{
	Name = other.Name + std::to_string(static_cast<uint16_t>(reinterpret_cast<uintptr_t>(this) & 0x0000'0000'FFFF'FFFF));
	ClientScript = other.ClientScript;
	ServerScript = other.ServerScript;
	Attributes = other.Attributes;
	Properties = other.Properties;
	Visible = other.Visible;
	ReplicateChanges = other.ReplicateChanges;
	IgnoresEvents = other.IgnoresEvents;
	Initialized = false;
	return *this;
}

void AnimatedSceneObject::SetAnimation(const std::string& image)
{
	auto dirty = Properties[Property::ANIMATION].Dirty;
	if (dirty)
	{
		// We've already been set to dirty and we are setting the animation again.
		// We are probably in a script loop.  Post again to try to clear it.
		auto anim = Properties.Get(Property::ANIMATION);
		anim->Set(image);
		anim->UpdateDispatch.Post(anim->ID);
	}
	else
	{
		// Hack to make continuous animations work.
		// If an animation is not set to continuous, then setting it to the same animation restarts it.
		const char* debugStr = image.c_str();
		if (boost::iends_with(image, ".gani") && Properties[Property::ANIMATION].GetAs<std::string>() == image)
		{
			// Set to dirty.  Gani animations can be restarted by re-setting the animation.
			auto anim = Properties.Get(Property::ANIMATION);
			anim->Dirty = true;
		}

		Properties[Property::ANIMATION] = image;
	}
}

///////////////////////////////////////////////////////////////////////////////

TMXSceneObject& TMXSceneObject::operator=(const TMXSceneObject& other)
{
	Name = other.Name + std::to_string(static_cast<uint16_t>(reinterpret_cast<uintptr_t>(this) & 0x0000'0000'FFFF'FFFF));
	ClientScript = other.ClientScript;
	ServerScript = other.ServerScript;
	Attributes = other.Attributes;
	Properties = other.Properties;
	Visible = other.Visible;
	ReplicateChanges = other.ReplicateChanges;
	IgnoresEvents = other.IgnoresEvents;
	Initialized = false;

	Tilesets = other.Tilesets;
	Layer = other.Layer;
	m_tmx = other.m_tmx;
	return *this;
}

Rectf TMXSceneObject::GetBounds() const noexcept
{
	return Rectf(GetPosition() + m_calculated_level_bounds.pos, m_calculated_level_bounds.size);
}

bool TMXSceneObject::LoadMap(const std::shared_ptr<tmx::Map>& map, const std::vector<std::pair<scene::TilesetGID, scene::TilesetPtr>>& tilesets) noexcept
{
	if (map == nullptr)
		return false;

	m_tmx = map;
	Tilesets = tilesets;
	RenderOrder = map->getRenderOrder();

	// We can't have a map without tilesets.
	if (Tilesets.empty())
		return false;

	// Get the map layer.
	const auto& layer = m_tmx->getLayers().at(Layer);
	if (layer->getType() != tmx::Layer::Type::Tile)
		return false;

	// Calculate chunks.
	const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();
	const auto& chunks = tilelayer.getChunks();
	for (size_t i = 0; i < chunks.size(); ++i)
	{
		const auto& chunk = chunks.at(i);

		Chunk ch{};
		ch.Position = Vector2di{ chunk.position.x, chunk.position.y };
		ch.Size = Vector2du{ static_cast<uint32_t>(chunk.size.x), static_cast<uint32_t>(chunk.size.y) };
		Chunks.emplace_back(std::move(ch));
	}

	// Calculate the level size.
	CalculateLevelSize();

	// Create the physics body configuration.
	physics::BodyConfiguration config{};
	config.Type = physics::BodyTypes::STATIC;
	config.BodyConf.UseFixedRotation(true);
	config.BodyConf.UseType(playrho::BodyType::Static);
	m_physics_body_config = std::make_optional(std::move(config));

	return true;
}

void TMXSceneObject::LoadChunkData(size_t chunk_idx, size_t chunks_max, Vector2di&& position, Vector2du&& size) noexcept
{
	if (chunk_idx >= Chunks.size())
		Chunks.resize(chunks_max);

	auto& chunkdef = Chunks.at(chunk_idx);
	chunkdef.Position = std::move(position);
	chunkdef.Size = std::move(size);
}

void TMXSceneObject::CalculateLevelSize() noexcept
{
	m_calculated_level_bounds = {};
	for (const auto& chunk : Chunks)
	{
		if (chunk.Position.x < m_calculated_level_bounds.pos.x)
			m_calculated_level_bounds.pos.x = static_cast<float>(chunk.Position.x);
		if (chunk.Position.y < m_calculated_level_bounds.pos.y)
			m_calculated_level_bounds.pos.y = static_cast<float>(chunk.Position.y);
		if (chunk.Size.x > m_calculated_level_bounds.size.x)
			m_calculated_level_bounds.size.x = static_cast<float>(chunk.Size.x);
		if (chunk.Size.y > m_calculated_level_bounds.size.y)
			m_calculated_level_bounds.size.y = static_cast<float>(chunk.Size.y);
	}

	if (Tilesets.empty())
		return;

	const auto& first_tileset = Tilesets.front().second;
	auto tilesize_x = first_tileset->TileDimensions.x;
	auto tilesize_y = first_tileset->TileDimensions.y;
	m_calculated_level_bounds.pos.x *= tilesize_x;
	m_calculated_level_bounds.pos.y *= tilesize_y;
	m_calculated_level_bounds.size.x *= tilesize_x;
	m_calculated_level_bounds.size.y *= tilesize_y;

	// log::PrintLine(log::game, "TMX [{}] Calculated level size as ({}, {}, {}, {}).", ID, m_calculated_level_bounds.pos.x, m_calculated_level_bounds.pos.y, m_calculated_level_bounds.size.x, m_calculated_level_bounds.size.y);
}

void TMXSceneObject::CalculateChunksToRender(const Rectf& window, std::vector<uint32_t>& chunks_to_render) const noexcept
{
	chunks_to_render.clear();
	const auto& first_tileset = Tilesets.front().second;
	for (uint32_t i = 0; i < Chunks.size(); ++i)
	{
		const auto& chunk = Chunks.at(i);
		if (math::containsOrIntersects(chunk.Position * first_tileset->TileDimensions, chunk.Size * first_tileset->TileDimensions, window))
			chunks_to_render.push_back(i);
	}
}

void TMXSceneObject::LoadChunkCollision(uint32_t chunk_idx, std::shared_ptr<scene::Scene> scene) noexcept
{
	if (chunk_idx >= Chunks.size() || m_tmx == nullptr) return;
	auto& chunkdef = Chunks.at(chunk_idx);
	if (chunkdef.CollisionLoaded)
		return;

	const auto& layer = m_tmx->getLayers().at(Layer);
	if (!layer || layer->getType() != tmx::Layer::Type::Tile)
		return;

	const auto& tilelayer = layer->getLayerAs<tmx::TileLayer>();
	const auto& chunks = tilelayer.getChunks();
	if (chunk_idx >= chunks.size())
		return;

	// Search all the tilesets to find the details on a tile.
	const auto& tilesets = m_tmx->getTilesets();
	const auto& tile_size = m_tmx->getTileSize();
	auto find_tile_in_tileset = [&tilesets](const tmx::TileLayer::Tile& layer_tile) -> const tmx::Tileset::Tile*
	{
		for (const auto& tileset : tilesets)
		{
			const auto* tile = tileset.getTile(layer_tile.ID);
			return tile;
		}
		return nullptr;
	};

	// Load the scene.
	if (scene == nullptr)
		scene = m_current_scene.lock();
	if (!scene) return;

	// If we have no body config, create it now.
	if (!m_physics_body_config.has_value())
	{
		// Create the physics body configuration.
		physics::BodyConfiguration config{};
		config.Type = physics::BodyTypes::STATIC;
		config.BodyConf.UseFixedRotation(true);
		config.BodyConf.UseType(playrho::BodyType::Static);
		m_physics_body_config = std::make_optional(std::move(config));
	}

	const auto& chunk = chunks.at(chunk_idx);
	auto ppu = scene->Physics.GetPixelsPerUnit();
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
		auto [x, y] = helper::getTilePosition(m_tmx->getRenderOrder(), chunk.size, i);

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
		clip.PreserveCollinear(false);
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
		for (const auto& polychild : partitioned_polys)
		{
			auto shape = helper::createPhysicsShape(m_physics_body_config.value(), chunk.position, ppu, polychild);
			m_shape_chunks.push_back(chunk_idx);
			if (shape.has_value() && HasPhysicsBody())
			{
				playrho::d2::FixtureConf fixture_conf{};
				fixture_conf.UseBody(m_physics_body.value()).UseShape(shape.value());
				auto fixture_id = playrho::d2::CreateFixture(scene->Physics.GetWorld(), fixture_conf);
				chunkdef.Fixtures.push_back(fixture_id);
			}
		}
	}

	chunkdef.CollisionLoaded = true;
	if (!HasPhysicsBody())
		ConstructPhysicsBodiesFromConfiguration();

	// log::PrintLine(log::game, "TMX [{}] Loaded collision for chunk {}.", ID, chunk_idx);
}

void TMXSceneObject::SetChunkCollision(uint32_t chunk_idx, const physics::BodyConfiguration& config) noexcept
{
	if (chunk_idx >= Chunks.size()) return;
	auto& chunkdef = Chunks.at(chunk_idx);
	if (chunkdef.CollisionLoaded)
		return;

	// Load the scene.
	auto scene = m_current_scene.lock();
	if (!scene) return;

	// If we don't have a physics body, create it normally.
	if (!HasPhysicsBody())
	{
		auto position = GetPosition();
		auto ppu = scene->Physics.GetPixelsPerUnit();

		physics::BodyConfiguration tmxbody
		{
			.Type = physics::BodyTypes::STATIC,
			.BodyConf = config.BodyConf,
			//.Shapes = config.Shapes,
		};
		tmxbody.BodyConf.UseFixedRotation(true);
		tmxbody.BodyConf.UseType(playrho::BodyType::Static);
		tmxbody.BodyConf.UseLocation({ position.x / ppu, position.y / ppu });

		auto body = playrho::d2::CreateBody(scene->Physics.GetWorld(), tmxbody.BodyConf);
		m_physics_body = body;
		scene->Physics.AddBodyToSceneObject(body, shared_from_this());
	}

	// We already have a body so link the shapes to the existing body.
	for (const auto& shape : config.Shapes)
	{
		playrho::d2::FixtureConf fixture_conf{};
		fixture_conf.UseBody(m_physics_body.value()).UseShape(shape);
		auto fixture_id = playrho::d2::CreateFixture(scene->Physics.GetWorld(), fixture_conf);
		chunkdef.Fixtures.push_back(fixture_id);
	}

	// Mark the chunk as loaded.
	chunkdef.CollisionLoaded = true;
}

void TMXSceneObject::DestroyAllPhysicsBodies() noexcept
{
	for (auto& chunk : Chunks)
		chunk.Fixtures.clear();

	SceneObject::DestroyAllPhysicsBodies();
}

void TMXSceneObject::addPhysicsBodyToScene(std::shared_ptr<scene::Scene>& scene) noexcept
{
	if (!m_physics_body_config.has_value() || m_physics_body_config.value().Shapes.empty())
		return;

	auto& world = scene->Physics.GetWorld();
	auto ppu = scene->Physics.GetPixelsPerUnit();
	auto position = GetPosition();

	// Update the config before we add it.
	physics::BodyConfiguration config{ m_physics_body_config.value()};
	config.BodyConf.UseType(playrho::BodyType::Static);
	config.BodyConf.UseLocation({ position.x / ppu, position.y / ppu });
	if (!m_attached_to.expired())
		config.BodyConf.enabled = false;

	// Create the body.
	auto body = playrho::d2::CreateBody(world, config.BodyConf);
	m_physics_body = body;
	scene->Physics.AddBodyToSceneObject(body, shared_from_this());

	// Add the shapes to the body.
	for (uint32_t i = 0; i < config.Shapes.size(); ++i)
	{
		playrho::d2::FixtureConf fixture_conf{};
		fixture_conf.UseBody(body).UseShape(config.Shapes[i]);
		auto fixture_id = playrho::d2::CreateFixture(world, fixture_conf);

		auto& chunk = Chunks.at(m_shape_chunks[i]);
		chunk.Fixtures.push_back(fixture_id);
	}
}

///////////////////////////////////////////////////////////////////////////////

const std::string& TextSceneObject::GetFont() const noexcept
{
	return m_font;
}

void TextSceneObject::SetFont(const std::string& font)
{
	m_font = font;

	// Dispatch text so the render component can get our font changes.
	auto text = Properties.Get(Property::TEXT);
	text->UpdateDispatch.Post(text->ID);
}

uint32_t TextSceneObject::GetFontSize() const noexcept
{
	return m_font_size;
}

void TextSceneObject::SetFontSize(uint32_t size)
{
	m_font_size = size;

	// Dispatch text so the render component can get our font changes.
	auto text = Properties.Get(Property::TEXT);
	text->UpdateDispatch.Post(text->ID);
}

bool TextSceneObject::GetCentered() const noexcept
{
	return m_centered;
}

void TextSceneObject::SetCentered(bool centered)
{
	m_centered = centered;
}

} // end namespace tdrp
