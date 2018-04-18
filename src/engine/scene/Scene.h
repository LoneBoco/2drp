#pragma once

#include <atomic>

#include "engine/common.h"

#include "engine/scene/SceneObject.h"

namespace tdrp::scene
{

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	Scene(const Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene& operator=(Scene&& other) = delete;

	uint32_t AddObject(std::shared_ptr<SceneObject> so);
	bool RemoveObject(std::shared_ptr<SceneObject> so);

	// uint32_t AddZone();
	// bool RemoveZone();

	std::weak_ptr<SceneObject> FindObject(uint32_t id);
	std::weak_ptr<SceneObject> FindObject(const std::string& name);

	std::atomic_bool IsLoading;

private:
	std::map<uint32_t, std::shared_ptr<SceneObject>> m_graph;
};

} // end namespace tdrp::scene
