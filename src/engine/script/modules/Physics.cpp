#include "engine/script/modules/bind.h"

#include "engine/scene/SceneObject.h"
#include "engine/scene/Scene.h"
#include "engine/physics/Physics.h"

#include <PlayRho/PlayRho.hpp>
#include <PlayRho/Common/Units.hpp>
#include <PlayRho/Dynamics/Contacts/Contact.hpp>
#include <PlayRho/Collision/Manifold.hpp>

using namespace playrho;


namespace tdrp::script::modules
{

namespace helpers::sceneobject
{
	static scene::ScenePtr check_body(SceneObject& so)
	{
		if (!so.HasPhysicsBody())
			return nullptr;
		return so.GetCurrentScene().lock();
	}
} // end namespace helpers::sceneobject

namespace functions::manifold
{
	static Vector2df get_normal(physics::Collision& collision)
	{
		return Vector2df{ collision.Manifold.GetNormal().GetX(), collision.Manifold.GetNormal().GetY() };
	}

	static std::vector<Vector2df> get_points(physics::Collision& collision)
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
	static bool get_enabled(physics::Collision& collision)
	{
		auto& contact = collision.World.GetContact(collision.Contact);
		return contact.IsEnabled();
	}

	static void set_enabled(physics::Collision& collision, bool value)
	{
		playrho::d2::SetEnabled(collision.World, collision.Contact, value);
	}
} // end namespace functions::contact

namespace functions::body
{
	static physics::BodyTypes get_type(physics::BodyConfiguration& body)
	{
		return body.Type;
	}

	static physics::BodyConfiguration& set_type(physics::BodyConfiguration& body, physics::BodyTypes type)
	{
		body.Type = type;
		return body;
	}

	static bool get_fixed_rotation(physics::BodyConfiguration& body)
	{
		return body.BodyConf.fixedRotation;
	}

	static physics::BodyConfiguration& set_fixed_rotation(physics::BodyConfiguration& body, bool fixed_rotation)
	{
		body.BodyConf.fixedRotation = fixed_rotation;
		return body;
	}

	static bool get_enabled(physics::BodyConfiguration& body)
	{
		return body.BodyConf.enabled;
	}

	static physics::BodyConfiguration& set_enabled(physics::BodyConfiguration& body, bool enabled)
	{
		body.BodyConf.enabled = enabled;
		return body;
	}

	static physics::BodyConfiguration& add_collision_circle(physics::BodyConfiguration& body, float radius, const Vector2df& position)
	{
		body.Shapes.emplace_back(playrho::d2::Shape{ playrho::d2::DiskShapeConf{ radius }.UseLocation({ position.x, position.y }).UseDensity(1010_kgpm2)});
		return body;
	}

	static physics::BodyConfiguration& add_collision_box(physics::BodyConfiguration& body, const Vector2df& radius, const Vector2df& center)
	{
		body.Shapes.emplace_back(playrho::d2::Shape{ playrho::d2::PolygonShapeConf{}.SetAsBox(radius.x, radius.y, { center.x, center.y}, 0) });
		return body;
	}
} // end namespace functions::body

namespace functions::sceneobject
{
	static bool get_enabled(SceneObject& so)
	{
		auto scene = helpers::sceneobject::check_body(so);
		if (!scene || !so.HasPhysicsBody()) return false;

		auto& world = scene->Physics.GetWorld();
		if (playrho::d2::IsEnabled(world, so.GetPhysicsBody().value()))
			return true;

		return false;
	}

	static void set_enabled(SceneObject& so, bool value)
	{
		auto scene = helpers::sceneobject::check_body(so);
		if (!scene || !so.HasPhysicsBody()) return;

		auto& world = scene->Physics.GetWorld();
		playrho::d2::SetEnabled(world, so.GetPhysicsBody().value(), value);
	}
} // end namespace functions::sceneobject

void bind_physics(sol::state& lua)
{
	lua.new_enum<physics::BodyTypes>("BodyType", {
		{ "STATIC", physics::BodyTypes::STATIC },
		{ "KINEMATIC", physics::BodyTypes::KINEMATIC },
		{ "DYNAMIC", physics::BodyTypes::DYNAMIC },
		{ "HYBRID", physics::BodyTypes::HYBRID }
	});

	lua.new_usertype<physics::Collision>("Collision", sol::no_constructor,
		"Normal", sol::readonly_property(&functions::manifold::get_normal),
		"Points", sol::readonly_property(&functions::manifold::get_points),
		"Enabled", sol::property(&functions::contact::get_enabled, &functions::contact::set_enabled)
	);

	lua.new_usertype<physics::BodyConfiguration>("PhysicsBody", sol::constructors<physics::BodyConfiguration()>(),
		"Type", sol::property(&functions::body::get_type, &functions::body::set_type),
		"FixedRotation", sol::property(&functions::body::get_fixed_rotation, &functions::body::set_fixed_rotation),
		"Enabled", sol::property(&functions::body::get_enabled, &functions::body::set_enabled),
		"AddCollisionCircle", &functions::body::add_collision_circle,
		"AddCollisionBox", &functions::body::add_collision_box
	);

	auto physics = lua.create_named_table("Physics");

	auto sceneobject = lua.create_table();
	sceneobject.set_function("IsEnabled", &functions::sceneobject::get_enabled);
	sceneobject.set_function("SetEnabled", &functions::sceneobject::set_enabled);

	physics["SceneObject"] = sceneobject;
}

} // end namespace tdrp::script::modules
