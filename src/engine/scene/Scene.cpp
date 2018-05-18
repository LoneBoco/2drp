#include "engine/scene/Scene.h"

namespace tdrp::scene
{

uint32_t Scene::AddObject(std::shared_ptr<SceneObject> so)
{
	auto p = m_graph.insert(std::make_pair(so->ID, so));
	return p.second;
}

bool Scene::RemoveObject(std::shared_ptr<SceneObject> so)
{
	auto p = m_graph.find(so->ID);
	if (p == m_graph.end())
		return true;

	if (p->second != so)
		return false;

	m_graph.erase(p);
	return true;
}

std::weak_ptr<SceneObject> Scene::FindObject(uint32_t id)
{
	auto p = m_graph.find(id);
	if (p == m_graph.end())
		return std::weak_ptr<SceneObject>();

	return std::weak_ptr<SceneObject>(p->second);
}

std::weak_ptr<SceneObject> Scene::FindObject(const std::string& name)
{
	for (auto p : m_graph)
	{
		if (p.second->Name == name)
			return std::weak_ptr<SceneObject>(p.second);
	}

	return std::weak_ptr<SceneObject>();
}

} // end namespace tdrp::scene