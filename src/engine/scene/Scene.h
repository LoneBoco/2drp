#pragma once

#include <atomic>

#include "engine/common.h"

#include "engine/scene/SceneObject.h"

// Declare so we can friend these.
namespace tdrp::server
{
	class Server;
}
namespace tdrp::loader
{
	class LevelLoader;
}

namespace tdrp::scene
{

class Scene
{
	friend class tdrp::server::Server;
	friend class tdrp::loader::LevelLoader;

public:
	Scene() = delete;
	Scene(const std::string& name) : m_name(name) {}
	~Scene() = default;

	Scene(const Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene& operator=(Scene&& other) = delete;
	bool operator==(const Scene& other) { return m_name == other.m_name; }

	uint32_t AddObject(std::shared_ptr<SceneObject> so);
	bool RemoveObject(std::shared_ptr<SceneObject> so);

	// uint32_t AddZone();
	// bool RemoveZone();

	std::weak_ptr<SceneObject> FindObject(uint32_t id) const;
	std::weak_ptr<SceneObject> FindObject(const std::string& name) const;

	const float GetTransmissionDistance() const;

	std::atomic_bool IsLoading;

protected:
	std::string m_name;
	std::map<uint32_t, std::shared_ptr<SceneObject>> m_graph;
	uint32_t m_transmission_distance = 2000;
};

inline const float Scene::GetTransmissionDistance() const
{
	return m_transmission_distance;
}

} // end namespace tdrp::scene
