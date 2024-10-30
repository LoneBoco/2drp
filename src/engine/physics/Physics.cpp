#include "engine/physics/Physics.h"
#include "engine/scene/SceneObject.h"

#include <PlayRho/Dynamics/Contacts/Contact.hpp>
#include <PlayRho/Collision/Manifold.hpp>


namespace tdrp::physics
{

constexpr playrho::d2::Velocity ZeroVelocity{};
constexpr playrho::Momentum2 ZeroMomentum{ 0, 0 };

Physics::Physics()
	: m_world{}
{
	using namespace std::placeholders;
	m_world.SetBeginContactListener(std::bind(&Physics::contactBegin, this, _1));
	m_world.SetEndContactListener(std::bind(&Physics::contactEnd, this, _1));
	m_world.SetPreSolveContactListener(std::bind(&Physics::preSolveContact, this, _1, _2));
	//m_world.SetPostSolveContactListener(std::bind(&Physics::postSolveContact, this, _1, _2, _3));
}

void Physics::ResetTimers()
{
}

void Physics::Update(const std::chrono::milliseconds& elapsed)
{
	auto step = playrho::StepConf{};
	step.deltaTime = elapsed.count() / 1000.0f;

	m_world.Step(step);

	// Update scene objects.
	for (const auto& body_id : m_world.GetBodies())
	{
		auto iter = m_scene_objects.find(body_id);
		if (iter == std::end(m_scene_objects))
			continue;

		auto so = iter->second.lock();
		if (!so) continue;

		auto transformation = playrho::d2::GetTransformation(m_world, body_id);
		auto velocity = playrho::d2::GetVelocity(m_world, body_id);
		auto acceleration = playrho::d2::GetAcceleration(m_world, body_id);

		so->Properties[Property::X] = transformation.p[0] * m_pixels_per_unit;
		so->Properties[Property::Y] = transformation.p[1] * m_pixels_per_unit;
		so->Properties[Property::ANGLE] = playrho::d2::GetAngle(m_world, body_id);
		so->Properties[Property::VELOCITY_X] = velocity.linear[0];
		so->Properties[Property::VELOCITY_Y] = velocity.linear[1];
		so->Properties[Property::VELOCITY_ANGLE] = velocity.angular;
		so->Properties[Property::ACCELERATION_X] = acceleration.linear[0];
		so->Properties[Property::ACCELERATION_Y] = acceleration.linear[1];
		so->Properties[Property::ACCELERATION_ANGLE] = acceleration.angular;
	}
}

void Physics::AddSceneObject(const std::shared_ptr<SceneObject>& so)
{
	so->ConstructPhysicsBodiesFromConfiguration();

	const auto& body = so->GetPhysicsBody();
	if (!body.has_value())
		return;

	m_scene_objects.insert_or_assign(body.value(), so);
}

void Physics::AddBodyToSceneObject(playrho::BodyID body, const std::shared_ptr<SceneObject>& so)
{
	m_scene_objects.insert_or_assign(body, so);
}

void Physics::RemoveSceneObject(const std::shared_ptr<SceneObject>& so)
{
	const auto& body = so->GetPhysicsBody();
	if (!body.has_value())
		return;

	m_scene_objects.erase(body.value());

	so->DestroyAllPhysicsBodies();
}

std::shared_ptr<SceneObject> Physics::FindSceneObjectByBodyId(playrho::BodyID bodyId)
{
	auto iter = m_scene_objects.find(bodyId);
	if (iter != std::end(m_scene_objects))
		return iter->second.lock();
	return nullptr;
}

///////////////////////////////////////////////////////////

void Physics::contactBegin(playrho::ContactID id)
{
	const auto& contact = m_world.GetContact(id);
	auto bodyA_ID = contact.GetBodyA();
	auto bodyB_ID = contact.GetBodyB();

	// Get SceneObjects from bodies.
	auto iterA = m_scene_objects.find(bodyA_ID);
	auto iterB = m_scene_objects.find(bodyB_ID);
	if (iterA == std::end(m_scene_objects) || iterB == std::end(m_scene_objects))
		return;

	auto soA = iterA->second.lock();
	auto soB = iterB->second.lock();
	if (!soA || !soB)
		return;
	
	// Call Lua function for collision.
	soA->OnCollision.RunAll(soB);
	soB->OnCollision.RunAll(soA);
}

void Physics::contactEnd(playrho::ContactID id)
{
	const auto& contact = m_world.GetContact(id);
	auto bodyA_ID = contact.GetBodyA();
	auto bodyB_ID = contact.GetBodyB();

	// Get SceneObjects from bodies.
	auto iterA = m_scene_objects.find(bodyA_ID);
	auto iterB = m_scene_objects.find(bodyB_ID);
	if (iterA == std::end(m_scene_objects) || iterB == std::end(m_scene_objects))
		return;

	auto soA = iterA->second.lock();
	auto soB = iterB->second.lock();
	if (!soA || !soB)
		return;

	auto world_manifold = playrho::d2::GetWorldManifold(m_world, id);
	Collision collision{ m_world, id, world_manifold };

	// Call Lua function for collision.
	soA->OnCollisionEnd.RunAll(soB, collision);
	soB->OnCollisionEnd.RunAll(soA, collision);
}

void Physics::preSolveContact(playrho::ContactID id, const playrho::d2::Manifold& manifold)
{
	const auto& contact = m_world.GetContact(id);
	auto bodyA_ID = contact.GetBodyA();
	auto bodyB_ID = contact.GetBodyB();

	// Get SceneObjects from bodies.
	auto iterA = m_scene_objects.find(bodyA_ID);
	auto iterB = m_scene_objects.find(bodyB_ID);
	if (iterA == std::end(m_scene_objects) || iterB == std::end(m_scene_objects))
		return;

	auto soA = iterA->second.lock();
	auto soB = iterB->second.lock();
	if (!soA || !soB)
		return;

	auto world_manifold = playrho::d2::GetWorldManifold(m_world, id);
	Collision collision{ m_world, id, world_manifold };

	// Call Lua function for pre-solving the contact.
	soA->OnSolveContact.RunAll(soB, &collision);
	soB->OnSolveContact.RunAll(soA, &collision);
}

void Physics::postSolveContact(playrho::ContactID id, const playrho::d2::ContactImpulsesList& impulses, unsigned step_iterations)
{
}

} // end namespace tdrp::physics
