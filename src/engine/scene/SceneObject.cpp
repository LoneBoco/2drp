// #include <Box2D.h>
#include <deque>
#include <set>

#include "engine/common.h"

#include "SceneObject.h"

namespace tdrp
{
/*
namespace physics
{

void Physics::add_collision_circle(float radius, const core::vector2df& center, float density, float friction, float restitution)
{
	b2CircleShape circle;
	circle.m_radius = radius;
	circle.m_p = b2Vec2(center.X, center.Y);

	b2FixtureDef fixture;
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;
	fixture.shape = &circle;

	Body->CreateFixture(&fixture);
}

void Physics::add_collision_box(const core::vector2df& dimension, const core::vector2df& center, float angle, float density, float friction, float restitution)
{
	b2PolygonShape poly;
	poly.SetAsBox(dimension.X, dimension.Y, b2Vec2(center.X, center.Y), angle);

	b2FixtureDef fixture;
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;
	fixture.shape = &poly;

	Body->CreateFixture(&fixture);
}

void Physics::add_collision_poly(const std::vector<core::vector2df>& vertices, float density, float friction, float restitution)
{
	b2PolygonShape poly;
	std::vector<b2Vec2> v;
	for (unsigned int i = 0; i < vertices.size(); ++i)
		v.push_back(b2Vec2(vertices[i].X, vertices[i].Y));
	poly.Set(v.data(), v.size());

	b2FixtureDef fixture;
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;
	fixture.shape = &poly;

	Body->CreateFixture(&fixture);
}

void Physics::add_collision_poly(const std::vector<b2Vec2>& vertices, float density, float friction, float restitution)
{
	b2PolygonShape poly;
	poly.Set(vertices.data(), vertices.size());

	b2FixtureDef fixture;
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;
	fixture.shape = &poly;

	Body->CreateFixture(&fixture);
}

void Physics::add_collision_edge(const core::vector2df& v1, const core::vector2df& v2, bool hasVertex0, const core::vector2df& v0, bool hasVertex3, const core::vector2df& v3, float density, float friction, float restitution)
{
	b2EdgeShape edge;
	edge.m_vertex1.Set(v1.X, v1.Y);
	edge.m_vertex2.Set(v2.X, v2.Y);
	if (hasVertex0)
	{
		edge.m_hasVertex0 = true;
		edge.m_vertex0.Set(v0.X, v0.Y);
	}
	if (hasVertex3)
	{
		edge.m_hasVertex3 = true;
		edge.m_vertex3.Set(v3.X, v3.Y);
	}

	b2FixtureDef fixture;
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;
	fixture.shape = &edge;

	Body->CreateFixture(&fixture);
}

} // end namespace physics
*/

SceneObject::SceneObject(const std::shared_ptr<ObjectClass> c, const uint32_t id)
: ID(id),
Visible(true), RenderVisible(false), // SceneGraph(),
m_object_class(c) /*UpdateCallback(nullptr), PhysicsUpdateCallback(nullptr),*/
{
	if (c)
	{
		Attributes = c->Attributes;
	}

	// Create our physics body.
	/*
	b2BodyDef def;
	def.userData = this;
	Physics.Body = CPhysicsManager::Instance().PhysicsWorld().CreateBody(&def);
	*/
}

SceneObject::~SceneObject()
{
	/*
	CPhysicsManager::Instance().PhysicsWorld().DestroyBody(Physics.Body);
	Physics.Body = nullptr;
	UpdateCallback = nullptr;
	*/
}

Vector2df SceneObject::GetPosition() const
{
	return Vector2df{
		Properties[Property::X].GetFloat(),
		Properties[Property::Y].GetFloat()
	};
}

void SceneObject::SetPosition(const Vector2df& position)
{
	// if (!Physics.Owner) return;
	Properties[Property::X] = position.x;
	Properties[Property::Y] = position.y;
}

int64_t SceneObject::GetDepth() const
{
	return Properties[Property::Z].GetSigned();
}

void SceneObject::SetDepth(int64_t z)
{
	Properties[Property::Z] = z;
}

float SceneObject::GetRotation() const
{
	return Properties[Property::ROTATION].GetFloat();
}

void SceneObject::SetRotation(float rotation)
{
	Properties[Property::ROTATION] = rotation;
}

Vector2df SceneObject::GetScale() const
{
	return Vector2df{
		Properties[Property::SCALE_X].GetFloat(),
		Properties[Property::SCALE_Y].GetFloat()
	};
}

void SceneObject::SetScale(const Vector2df& scale)
{
	Properties[Property::SCALE_X] = scale.x;
	Properties[Property::SCALE_Y] = scale.y;

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

void SceneObject::Update()
{
	/*
	// Update our physics engine if we own this object.
	if (Physics.Owner)
	{
		uint8_t ppu = CPhysicsManager::Instance().get_pixels_per_unit();

		// Check for manual X/Y changes.
		float x = Properties.get(EP_X)->get_as_float();
		float y = Properties.get(EP_Y)->get_as_float();
		bool x_changed = (ChangedProperties.find(EP_X) != ChangedProperties.end());
		bool y_changed = (ChangedProperties.find(EP_Y) != ChangedProperties.end());
		if (x_changed || y_changed)
			Physics.Body->SetTransform(b2Vec2(Properties.get(EP_X)->get_as_float() / (float)ppu, Properties.get(EP_Y)->get_as_float() / (float)ppu), Physics.Body->GetAngle());

		// Check for velocity changes.
		float velocity[] = { Properties.get(EP_VELOCITY_X)->get_as_float(), Properties.get(EP_VELOCITY_Y)->get_as_float() };
		b2Vec2 v = Physics.Body->GetLinearVelocity();
		if (v.x != velocity[0])
		{
			Properties.get(EP_VELOCITY_X)->set(velocity[0]);
			ChangedProperties.insert(EP_VELOCITY_X);
		}
		if (v.y != velocity[1])
		{
			Properties.get(EP_VELOCITY_Y)->set(velocity[1]);
			ChangedProperties.insert(EP_VELOCITY_Y);
		}

		// Check for force changes.
		float force[] = { Properties.get(EP_FORCE_X)->get_as_float(), Properties.get(EP_FORCE_Y)->get_as_float() };
		b2Vec2 f = Physics.Body->GetForce();
		if (f.x != force[0])
		{
			Properties.get(EP_FORCE_X)->set(force[0]);
			ChangedProperties.insert(EP_FORCE_X);
		}
		if (f.y != force[1])
		{
			Properties.get(EP_FORCE_Y)->set(force[1]);
			ChangedProperties.insert(EP_FORCE_Y);
		}

		// Check for torque changes.
		float torque = Properties.get(EP_TORQUE)->get_as_float();
		float t = Physics.Body->GetTorque();
		if (t != torque)
		{
			Properties.get(EP_TORQUE)->set(t);
			ChangedProperties.insert(EP_TORQUE);
		}

		// Check for rotation changes.
		float rotation = Properties.get(EP_ROTATION)->get_as_float();
		float r = -Physics.Body->GetAngle() * core::RADTODEG;
		if (r != rotation)
		{
			Properties.get(EP_ROTATION)->set(r);
			ChangedProperties.insert(EP_ROTATION);
		}
	}

	// Call our callback function.
	if (UpdateCallback != nullptr)
		(*UpdateCallback)(this);
	*/
}

/*
void SceneObject::update_physics()
{
	// Update our position if the physics engine changed our location.
	uint8_t ppu = CPhysicsManager::Instance().get_pixels_per_unit();
	b2Vec2 pos = Physics.Body->GetPosition();
	if (pos.x != PreviousPhysicsPosition.x)
	{
		Properties.get(EP_X)->set(pos.x * ppu);
		ChangedProperties.insert(EP_X);
	}
	if (pos.y != PreviousPhysicsPosition.y)
	{
		Properties.get(EP_Y)->set(pos.y * ppu);
		ChangedProperties.insert(EP_Y);
	}
	PreviousPhysicsPosition = pos;

	// Call our callback function.
	if (PhysicsUpdateCallback != nullptr)
		(*PhysicsUpdateCallback)(this);
}
*/

} // end namespace tdrp
