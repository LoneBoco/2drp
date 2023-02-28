#pragma once

#include <PlayRho/PlayRho.hpp>

#include "engine/common.h"


namespace tdrp
{
	class SceneObject;
}

namespace tdrp::physics
{

enum class ShapeTypes
{
	POLYGON = 0,
	CIRCLE,
	
	COUNT
};

struct Collision
{
	playrho::d2::World& World;
	playrho::ContactID Contact;
	playrho::d2::WorldManifold Manifold;
};


class Physics
{
public:
	Physics();
	~Physics() = default;

	Physics(const Physics& other) = delete;
	Physics(Physics&& other) = delete;
	Physics& operator=(const Physics& other) = delete;
	Physics& operator=(Physics&& other) = delete;
	bool operator==(const Physics& other) = delete;

public:
	playrho::d2::World& GetWorld();
	uint8_t GetPixelsPerUnit() const;
	void SetPixelsPerUnit(uint8_t pixels);

public:
	void ResetTimers();
	void Update(const std::chrono::milliseconds& elapsed);

public:
	void AddSceneObject(const std::shared_ptr<SceneObject>& so);
	void RemoveSceneObject(const std::shared_ptr<SceneObject>& so);
	std::shared_ptr<SceneObject> FindSceneObjectByBodyId(playrho::BodyID bodyId);

protected:
	void contactBegin(playrho::ContactID);
	void contactEnd(playrho::ContactID);
	void preSolveContact(playrho::ContactID, const playrho::d2::Manifold&);

protected:
	playrho::d2::World m_world;
	uint8_t m_pixels_per_unit = 16;
	std::unordered_map<playrho::BodyID, std::weak_ptr<SceneObject>> m_scene_objects;
};


inline playrho::d2::World& Physics::GetWorld()
{
	return m_world;
}

inline uint8_t Physics::GetPixelsPerUnit() const
{
	return m_pixels_per_unit;
}

inline void Physics::SetPixelsPerUnit(uint8_t pixels)
{
	m_pixels_per_unit = pixels;
}

} // end namespace tdrp::physics
