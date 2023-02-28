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

using SearchPredicate = std::function<bool(SceneObjectPtr&)>;

class Scene : public std::enable_shared_from_this<Scene>
{
	friend class tdrp::server::Server;
	friend class tdrp::Loader;

public:
	Scene() = delete;
	Scene(const std::string& name) noexcept : m_name(name) {}
	~Scene() = default;

	Scene(const Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene& operator=(Scene&& other) = delete;
	bool operator==(const Scene& other) noexcept { return m_name == other.m_name; }

public:
	const std::string& GetName() const noexcept;

	bool AddObject(SceneObjectPtr so) noexcept;
	bool RemoveObject(SceneObjectPtr so) noexcept;
	
	// uint32_t AddZone();
	// bool RemoveZone();

	SceneObjectPtr FindObject(SceneObjectID id) const noexcept;
	SceneObjectPtr FindObject(const std::string& name) const noexcept;

	std::unordered_map<SceneObjectID, SceneObjectPtr>& GetGraph() noexcept;

	std::vector<SceneObjectPtr> FindObjectsInRangeOf(const Vector2df& position, float radius, SearchPredicate = {}) noexcept;
	std::vector<SceneObjectPtr> FindObjectsBoundInRangeOf(const Vector2df& position, float radius, SearchPredicate = {}) noexcept;
	std::vector<SceneObjectPtr> FindObjectsInRectangle(const Recti& rectangle, SearchPredicate = {}) noexcept;
	std::vector<SceneObjectPtr> FindObjectsBoundInRectangle(const Rectf& rectangle, SearchPredicate = {}) noexcept;
	
	std::vector<SceneObjectPtr> FindObjectsByCollision(const Vector2df& position, float radius, SearchPredicate = {}) noexcept;
	std::vector<SceneObjectPtr> FindObjectsByCollisionAndBoundInRange(const Vector2df& position, float radius, SearchPredicate = {}) noexcept;

	const uint32_t GetTransmissionDistance() const noexcept;

	physics::Physics Physics;

	std::atomic_bool IsLoading;

protected:
	std::string m_name;
	std::unordered_map<SceneObjectID, SceneObjectPtr> m_graph;
	uint32_t m_transmission_distance = 6000;
};
using ScenePtr = std::shared_ptr<Scene>;

inline const std::string& Scene::GetName() const noexcept
{
	return m_name;
}

inline std::unordered_map<uint32_t, SceneObjectPtr>& Scene::GetGraph() noexcept
{
	return m_graph;
}

inline const uint32_t Scene::GetTransmissionDistance() const noexcept
{
	return m_transmission_distance;
}

} // end namespace tdrp::scene
