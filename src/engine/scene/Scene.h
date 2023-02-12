#pragma once

#include <atomic>

#include "engine/common.h"

#include "engine/scene/SceneObject.h"
#include "engine/physics/Physics.h"

// Declare so we can friend these.
namespace tdrp::server
{
	class Server;
}
namespace tdrp
{
	class Loader;
}

namespace tdrp::scene
{

class Scene : public std::enable_shared_from_this<Scene>
{
	friend class tdrp::server::Server;
	friend class tdrp::Loader;

public:
	Scene() = delete;
	Scene(const std::string& name) : m_name(name) {}
	~Scene() = default;

	Scene(const Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene& operator=(Scene&& other) = delete;
	bool operator==(const Scene& other) { return m_name == other.m_name; }

public:
	const std::string& GetName() const;

	bool AddObject(SceneObjectPtr so);
	bool RemoveObject(SceneObjectPtr so);
	
	// uint32_t AddZone();
	// bool RemoveZone();

	SceneObjectPtr FindObject(SceneObjectID id) const;
	SceneObjectPtr FindObject(const std::string& name) const;

	std::unordered_map<SceneObjectID, SceneObjectPtr>& GetGraph();

	std::vector<SceneObjectPtr> FindObjectsInRangeOf(const Vector2df& position, float radius);
	std::vector<SceneObjectPtr> FindObjectsBoundInRangeOf(const Vector2df& position, float radius);
	std::vector<SceneObjectPtr> FindObjectsInRectangle(const Recti& rectangle);
	std::vector<SceneObjectPtr> FindObjectsBoundInRectangle(const Rectf& rectangle);

	const uint32_t GetTransmissionDistance() const;

	physics::Physics Physics;

	std::atomic_bool IsLoading;

protected:
	std::string m_name;
	std::unordered_map<SceneObjectID, SceneObjectPtr> m_graph;
	uint32_t m_transmission_distance = 6000;
};
using ScenePtr = std::shared_ptr<Scene>;

inline const std::string& Scene::GetName() const
{
	return m_name;
}

inline std::unordered_map<uint32_t, SceneObjectPtr>& Scene::GetGraph()
{
	return m_graph;
}

inline const uint32_t Scene::GetTransmissionDistance() const
{
	return m_transmission_distance;
}

} // end namespace tdrp::scene
