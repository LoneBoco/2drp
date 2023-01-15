#include "engine/script/modules/bind.h"

#include "engine/scene/SceneObject.h"
#include "engine/scene/Scene.h"
#include "engine/physics/Physics.h"

#include <PlayRho/PlayRho.hpp>
#include <PlayRho/Dynamics/Contacts/Contact.hpp>
#include <PlayRho/Collision/Manifold.hpp>


namespace tdrp::script::modules
{

namespace helpers::sceneobject
{
	scene::ScenePtr check_body(SceneObject& so)
	{
		auto body = so.GetInitializedPhysicsBody();
		if (!body.has_value()) return nullptr;

		return so.GetCurrentScene().lock();
	}
} // end namespace helpers::sceneobject

namespace functions::manifold
{
	Vector2df get_normal(physics::Collision& collision)
	{
		return Vector2df{ collision.Manifold.GetNormal().GetX(), collision.Manifold.GetNormal().GetY() };
	}

	std::vector<Vector2df> get_points(physics::Collision& collision)
	{
		std::vector<Vector2df> result;

		for (playrho::d2::WorldManifold::size_type points = 0; points < collision.Manifold.GetPointCount(); ++points)
		{
			auto contact_point = collision.Manifold.GetPoint(points);
			result.emplace_back(contact_point[0], contact_point[1]);
		}

		return result;
	}
} // end namespace functions::manifold

namespace functions::contact
{
	bool get_enabled(physics::Collision& collision)
	{
		auto& contact = collision.World.GetContact(collision.Contact);
		return contact.IsEnabled();
	}

	void set_enabled(physics::Collision& collision, bool value)
	{
		playrho::d2::SetEnabled(collision.World, collision.Contact, value);
	}
} // end namespace functions::contact

namespace functions::sceneobject
{
	void set_type(SceneObject& so, playrho::BodyType type)
	{
		auto scene = helpers::sceneobject::check_body(so);
		if (!scene) return;

		auto& world = scene->Physics.GetWorld();
		const auto& body = so.PhysicsBody.value();
		playrho::d2::SetType(world, body, type);

		so.PhysicsChanged = true;
	}

	void set_fixedrotation(SceneObject& so, bool value)
	{
		auto scene = helpers::sceneobject::check_body(so);
		if (!scene) return;

		auto& world = scene->Physics.GetWorld();
		const auto& body = so.PhysicsBody.value();
		playrho::d2::SetFixedRotation(world, body, value);

		so.PhysicsChanged = true;
	}

	void add_collision_circle(SceneObject& so, float radius, const Vector2df& position)
	{
		auto scene = helpers::sceneobject::check_body(so);
		if (!scene) return;

		auto& world = scene->Physics.GetWorld();
		const auto& body = so.PhysicsBody.value();

		auto shape = playrho::d2::CreateShape(world, playrho::d2::DiskShapeConf{ radius }.UseLocation({ position.x, position.y }));
		playrho::d2::Attach(world, body, shape);

		so.PhysicsChanged = true;
	}

	void add_collision_box(SceneObject& so, const Vector2df& radius, const Vector2df& center)
	{
		auto scene = helpers::sceneobject::check_body(so);
		if (!scene) return;

		auto& world = scene->Physics.GetWorld();
		const auto& body = so.PhysicsBody.value();

		auto shape = playrho::d2::CreateShape(world, playrho::d2::PolygonShapeConf{ radius.x, radius.y }.Translate({ center.x, center.y }));
		playrho::d2::Attach(world, body, shape);

		so.PhysicsChanged = true;
	}

	bool get_enabled(SceneObject& so)
	{
		auto scene = helpers::sceneobject::check_body(so);
		if (!scene) return false;

		auto& world = scene->Physics.GetWorld();
		const auto& body = so.PhysicsBody.value();

		auto b = playrho::d2::GetBody(world, body);
		return b.IsEnabled();
	}

	void set_enabled(SceneObject& so, bool value)
	{
		auto scene = helpers::sceneobject::check_body(so);
		if (!scene) return;

		auto& world = scene->Physics.GetWorld();
		const auto& body = so.PhysicsBody.value();

		playrho::d2::SetEnabled(world, body, value);
	}
} // end namespace functions::sceneobject


void bind_physics(sol::state& lua)
{
	lua.new_enum<playrho::BodyType>("BodyType", {
		{ "STATIC", playrho::BodyType::Static },
		{ "KINEMATIC", playrho::BodyType::Kinematic },
		{ "DYNAMIC", playrho::BodyType::Dynamic }
	});

	lua.new_usertype<physics::Collision>("Collision", sol::no_constructor,
		"Normal", sol::readonly_property(&functions::manifold::get_normal),
		"Points", sol::readonly_property(&functions::manifold::get_points),
		"Enabled", sol::property(&functions::contact::get_enabled, &functions::contact::set_enabled)
	);

	auto physics = lua.create_named_table("Physics");
	
	auto sceneobject = lua.create_table();
	sceneobject.set_function("SetType", &functions::sceneobject::set_type);
	sceneobject.set_function("SetFixedRotation", &functions::sceneobject::set_fixedrotation);
	sceneobject.set_function("AddCollisionCircle", &functions::sceneobject::add_collision_circle);
	sceneobject.set_function("AddCollisionBox", &functions::sceneobject::add_collision_box);
	sceneobject.set_function("IsEnabled", &functions::sceneobject::get_enabled);
	sceneobject.set_function("SetEnabled", &functions::sceneobject::set_enabled);

	physics["SceneObject"] = sceneobject;
}

} // end namespace tdrp::script::modules
