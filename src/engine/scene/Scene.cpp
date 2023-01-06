#include "engine/scene/Scene.h"

namespace tdrp::scene
{

bool Scene::AddObject(SceneObjectPtr so)
{
	if (so->SetCurrentScene(shared_from_this()))
	{
		auto p = m_graph.insert_or_assign(so->ID, so);
		Physics.AddSceneObject(so);
		return true;
	}

	return false;
}

bool Scene::RemoveObject(SceneObjectPtr so)
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

SceneObjectPtr Scene::FindObject(SceneObjectID id) const
{
	auto p = m_graph.find(id);
	if (p == m_graph.end())
		return {};

	return p->second;
}

SceneObjectPtr Scene::FindObject(const std::string& name) const
{
	for (auto& p : m_graph)
	{
		if (p.second->Name == name)
			return p.second;
	}

	return {};
}

std::vector<SceneObjectPtr> Scene::FindObjectsInRangeOf(const Vector2df& position, float radius)
{
	std::vector<SceneObjectPtr> result;

	for (auto& p : m_graph)
	{
		if (Vector2df::Distance(p.second->GetPosition(), position) <= radius)
			result.push_back(p.second);
	}

	return result;
}

std::vector<SceneObjectPtr> Scene::FindObjectsBoundInRangeOf(const Vector2df& position, float radius)
{
	std::vector<SceneObjectPtr> result;

	for (auto& p : m_graph)
	{
		if (math::containsOrIntersects(p.second->GetBounds(), position, radius))
			result.push_back(p.second);
	}

	return result;
}

std::vector<SceneObjectPtr> Scene::FindObjectsInRectangle(const Recti& rectangle)
{
	std::vector<SceneObjectPtr> result;

	for (auto& p : m_graph)
	{
		if (math::contains(p.second->GetPosition(), rectangle))
			result.push_back(p.second);
	}

	return result;
}

std::vector<SceneObjectPtr> Scene::FindObjectsBoundInRectangle(const Rectf& rectangle)
{
	std::vector<SceneObjectPtr> result;

	for (auto& p : m_graph)
	{
		if (math::containsOrIntersects(p.second->GetBounds(), rectangle))
			result.push_back(p.second);
	}

	return result;
}

} // end namespace tdrp::scene
