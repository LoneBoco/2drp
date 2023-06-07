// #include <Box2D.h>
#include <deque>
#include <set>
#include <sstream>

#include "engine/common.h"
#include "engine/server/Player.h"
#include "engine/scene/Scene.h"
#include "engine/filesystem/Log.h"

#include "SceneObject.h"

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
	Replicated = other.Replicated;
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

	if (PhysicsBody.has_value())
	{
		if (auto scene = m_current_scene.lock(); scene)
		{
			auto ppu = scene->Physics.GetPixelsPerUnit();
			auto& world = scene->Physics.GetWorld();
			auto attached_to = m_attached_to.lock();

			if (!attached_to)
				playrho::d2::SetLocation(world, PhysicsBody.value(), { position.x / ppu, position.y / ppu });
			else
			{
				Vector2df attached_pos{ attached_to->GetPosition() + position };
				playrho::d2::SetLocation(world, PhysicsBody.value(), { attached_pos.x / ppu, attached_pos.y / ppu });
			}
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

	if (PhysicsBody.has_value())
	{
		if (auto scene = m_current_scene.lock(); scene)
		{
			auto& world = scene->Physics.GetWorld();
			playrho::d2::SetAngle(world, PhysicsBody.value(), angle);
		}
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
	if (PhysicsBody.has_value())
	{
		if (auto scene = m_current_scene.lock(); scene)
		{
			auto& world = scene->Physics.GetWorld();
		}
	}

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

	if (PhysicsBody.has_value())
	{
		if (auto scene = m_current_scene.lock(); scene)
		{
			auto& world = scene->Physics.GetWorld();
			playrho::d2::SetVelocity(world, PhysicsBody.value(), { velocity.x, velocity.y });
		}
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

	if (PhysicsBody.has_value())
	{
		if (auto scene = m_current_scene.lock(); scene)
		{
			auto& world = scene->Physics.GetWorld();
			playrho::d2::SetAcceleration(world, PhysicsBody.value(), { acceleration.x, acceleration.y });
		}
	}
}

float SceneObject::GetVelocityAngle() const
{
	return Properties[Property::VELOCITY_ANGLE].GetAs<float>();
}

void SceneObject::SetVelocityAngle(const float& velocity)
{
	Properties[Property::VELOCITY_ANGLE] = velocity;

	if (PhysicsBody.has_value())
	{
		if (auto scene = m_current_scene.lock(); scene)
		{
			auto& world = scene->Physics.GetWorld();
			playrho::d2::SetVelocity(world, PhysicsBody.value(), velocity);
		}
	}
}

float SceneObject::GetAccelerationAngle() const
{
	return Properties[Property::ACCELERATION_ANGLE].GetAs<float>();
}

void SceneObject::SetAccelerationAngle(const float& acceleration)
{
	Properties[Property::ACCELERATION_ANGLE] = acceleration;

	if (PhysicsBody.has_value())
	{
		if (auto scene = m_current_scene.lock(); scene)
		{
			auto& world = scene->Physics.GetWorld();
			playrho::d2::SetAcceleration(world, PhysicsBody.value(), acceleration);
		}
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

Rectf SceneObject::GetBounds() const
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
	if (m_owning_player.expired()) return false;
	if (auto owner = m_owning_player.lock(); owner)
		return owner->GetPlayerId() == id;
	return false;
}

void SceneObject::SetOwningPlayer(std::shared_ptr<server::Player> player)
{
	m_owning_player = player;
}

bool SceneObject::SetCurrentScene(std::shared_ptr<scene::Scene> scene)
{
	if (IsGlobal() || m_current_scene.expired())
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

	// If we are detaching, set our position to our current location.
	// If we are attaching, set our position offset to 0,0.
	if (other == nullptr)
	{
		log::PrintLine(":: Detaching scene object {}.", ID);
		SetPosition(other->GetPosition() + GetPosition());
	}
	else
	{
		log::PrintLine(":: Attaching scene object {} to {}.", ID, other->ID);
		SetPosition({ 0, 0 });
	}

	OnAttached.RunAll(m_attached_to, old);
}

SceneObjectID SceneObject::GetAttachedId()
{
	if (m_attached_to.expired()) return 0;
	auto attached = m_attached_to.lock();
	if (!attached) return 0;
	return attached->ID;
}

std::optional<playrho::BodyID> SceneObject::GetInitializedPhysicsBody()
{
	if (PhysicsBody.has_value())
		return PhysicsBody.value();

	auto scene = m_current_scene.lock();
	if (!scene) return std::nullopt;

	auto& world = scene->Physics.GetWorld();
	auto ppu = scene->Physics.GetPixelsPerUnit();
	auto position = GetPosition();

	playrho::d2::BodyConf config;
	config
		.UseType(playrho::BodyType::Kinematic)
		.UseLocation({ position.x / ppu, position.y / ppu })
		.UseFixedRotation(true);

	auto body = playrho::d2::CreateBody(world, config);
	PhysicsBody = body;

	return body;
}

void SceneObject::SynchronizePhysics()
{
	if (!PhysicsBody.has_value()) return;

	bool x_dirty = Properties[Property::X].IsDirty(AttributeDirty::CLIENT);
	bool y_dirty = Properties[Property::Y].IsDirty(AttributeDirty::CLIENT);
	if (x_dirty || y_dirty)
		SetPosition(GetPosition());

	bool angle = Properties[Property::ANGLE].IsDirty(AttributeDirty::CLIENT);
	if (angle)
		SetAngle(GetAngle());

	bool x_velocity = Properties[Property::VELOCITY_X].IsDirty(AttributeDirty::CLIENT);
	bool y_velocity = Properties[Property::VELOCITY_Y].IsDirty(AttributeDirty::CLIENT);
	if (x_velocity || y_velocity)
		SetVelocity(GetVelocity());

	bool x_acceleration = Properties[Property::ACCELERATION_X].IsDirty(AttributeDirty::CLIENT);
	bool y_acceleration = Properties[Property::ACCELERATION_Y].IsDirty(AttributeDirty::CLIENT);
	if (x_acceleration || y_acceleration)
		SetAcceleration(GetAcceleration());

	bool angle_velocity = Properties[Property::VELOCITY_ANGLE].IsDirty(AttributeDirty::CLIENT);
	if (angle_velocity)
		SetVelocityAngle(GetVelocityAngle());

	bool angle_acceleration = Properties[Property::ACCELERATION_ANGLE].IsDirty(AttributeDirty::CLIENT);
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
	return *this;
}

void AnimatedSceneObject::SetAnimation(const std::string& image)
{
	auto dirty = Properties[Property::ANIMATION].IsDirty(AttributeDirty::CLIENT);
	if (dirty)
	{
		// We've already been set to dirty and we are setting the animation again.
		// We are probably in a script loop.  Post again to try to clear it.
		auto anim = Properties.Get(Property::ANIMATION);
		anim->Set(image);
		anim->ClientUpdate.UpdateDispatch.Post(anim->ID);
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
			anim->SetAllDirty(true);
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
	TmxMap = other.TmxMap;
	Layer = other.Layer;
	return *this;
}

Rectf TMXSceneObject::GetBounds() const
{
	if (TmxMap == nullptr)
		return Rectf(GetPosition(), Vector2df(0.0f));

	return Rectf(GetPosition(), ChunkSize);
}

///////////////////////////////////////////////////////////////////////////////

const std::string& TextSceneObject::GetFont() const
{
	return m_font;
}

void TextSceneObject::SetFont(const std::string& font)
{
	m_font = font;

	// Dispatch text so the render component can get our font changes.
	auto text = Properties.Get(Property::TEXT);
	text->ClientUpdate.UpdateDispatch.Post(text->ID);
}

uint32_t TextSceneObject::GetFontSize() const
{
	return m_font_size;
}

void TextSceneObject::SetFontSize(uint32_t size)
{
	m_font_size = size;

	// Dispatch text so the render component can get our font changes.
	auto text = Properties.Get(Property::TEXT);
	text->ClientUpdate.UpdateDispatch.Post(text->ID);
}

bool TextSceneObject::GetCentered() const
{
	return m_centered;
}

void TextSceneObject::SetCentered(bool centered)
{
	m_centered = centered;
}

} // end namespace tdrp
