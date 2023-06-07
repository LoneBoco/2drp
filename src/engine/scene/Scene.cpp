#include "engine/scene/Scene.h"

#include <PlayRho/Collision/Shapes/Shape.hpp>
#include <PlayRho/Collision/Manifold.hpp>
#include <PlayRho/Collision/DynamicTree.hpp>
#include <PlayRho/Dynamics/Contacts/Contact.hpp>

namespace tdrp::scene
{

bool Scene::AddObject(SceneObjectPtr so) noexcept
{
	if (so->SetCurrentScene(shared_from_this()))
	{
		auto p = m_graph.insert_or_assign(so->ID, so);
		Physics.AddSceneObject(so);
		return true;
	}

	return false;
}

bool Scene::RemoveObject(SceneObjectPtr so) noexcept
{
	auto p = m_graph.find(so->ID);
	if (p == m_graph.end())
		return true;

	if (p->second != so)
		return false;

	m_graph.erase(p);
	p->second->SetCurrentScene(nullptr);

	Physics.RemoveSceneObject(so);

	return true;
}

SceneObjectPtr Scene::FindObject(SceneObjectID id) const noexcept
{
	auto p = m_graph.find(id);
	if (p == m_graph.end())
		return {};

	return p->second;
}

SceneObjectPtr Scene::FindObject(const std::string& name) const noexcept
{
	for (auto& p : m_graph)
	{
		if (boost::iequals(p.second->Name, name))
			return p.second;
	}

	return {};
}

std::vector<SceneObjectPtr> Scene::FindObjectsInRangeOf(const Vector2df& position, float radius, SearchPredicate predicate) noexcept
{
	std::vector<SceneObjectPtr> result;

	for (auto& p : m_graph)
	{
		if (Vector2df::Distance(p.second->GetPosition(), position) <= radius && (!predicate || predicate(p.second)))
			result.push_back(p.second);
	}

	return result;
}

std::vector<SceneObjectPtr> Scene::FindObjectsBoundInRangeOf(const Vector2df& position, float radius, SearchPredicate predicate) noexcept
{
	std::vector<SceneObjectPtr> result;

	for (auto& p : m_graph)
	{
		if (math::containsOrIntersects(p.second->GetBounds(), position, radius) && (!predicate || predicate(p.second)))
			result.push_back(p.second);
	}

	return result;
}

std::vector<SceneObjectPtr> Scene::FindObjectsInRectangle(const Recti& rectangle, SearchPredicate predicate) noexcept
{
	std::vector<SceneObjectPtr> result;

	for (auto& p : m_graph)
	{
		if (math::contains(p.second->GetPosition(), rectangle) && (!predicate || predicate(p.second)))
			result.push_back(p.second);
	}

	return result;
}

std::vector<SceneObjectPtr> Scene::FindObjectsBoundInRectangle(const Rectf& rectangle, SearchPredicate predicate) noexcept
{
	std::vector<SceneObjectPtr> result;

	for (auto& p : m_graph)
	{
		if (math::containsOrIntersects(p.second->GetBounds(), rectangle) && (!predicate || predicate(p.second)))
			result.push_back(p.second);
	}

	return result;
}

std::vector<SceneObjectPtr> Scene::FindObjectsByCollision(const Vector2df& position, float radius, SearchPredicate predicate) noexcept
{
	std::vector<SceneObjectPtr> results;

	auto ppu = Physics.GetPixelsPerUnit();
	auto& world = Physics.GetWorld();

	// Create the circle.
	auto shape = playrho::d2::Shape{ playrho::d2::DiskShapeConf{ radius / ppu } };
	auto transform = playrho::d2::Transformation{ playrho::Length2{ position.x / ppu, position.y / ppu } };

	// Bind the bounding box for the circle.
	auto aabb = playrho::d2::ComputeAABB(shape, transform);

	// Query the circle against the world.
	playrho::d2::Query(world.GetTree(), aabb,
		[&, this](playrho::BodyID bodyId, playrho::ShapeID shapeId, playrho::ChildCounter child) -> bool
		{
			auto shapeA = playrho::d2::GetChild(shape, 0);
			auto shapeB = playrho::d2::GetChild(playrho::d2::GetShape(world, shapeId), 0);
			const auto& body = playrho::d2::GetBody(world, bodyId);
			const auto& transformB = body.GetTransformation();

			auto manifold = playrho::d2::CollideShapes(shapeA, transform, shapeB, transformB);
			if (manifold.GetPointCount() != 0)
			{
				if (auto so = Physics.FindSceneObjectByBodyId(bodyId); so != nullptr && (!predicate || predicate(so)))
					results.push_back(so);
			}

			return true;
		});

	return results;
}

std::vector<SceneObjectPtr> Scene::FindObjectsByCollisionAndBoundInRange(const Vector2df& position, float radius, SearchPredicate predicate) noexcept
{
	// Collect objects from bothy methods.
	auto result = FindObjectsBoundInRangeOf(position, radius, [](SceneObjectPtr& so) { return !so->PhysicsBody.has_value(); });
	result.append_range(FindObjectsByCollision(position, radius));
	
	// Sort the vector.
	std::ranges::sort(result);

	// Remove duplicates.
	const auto ret = std::ranges::unique(result);
	result.erase(ret.begin(), ret.end());

	// Remove by predicate.
	if (predicate) [[likely]]
	{
		const auto ret2 = std::ranges::remove_if(result, [&predicate](SceneObjectPtr& so) { return !predicate(so); });
		result.erase(ret2.begin(), ret2.end());
	}

	return result;
}

} // end namespace tdrp::scene
