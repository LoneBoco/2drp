#pragma once

#include "engine/common.h"
#include "engine/scene/SceneObject.h"
#include "engine/helper/math.h"

namespace tdrp::camera
{

class Camera
{
public:
	Camera() : m_camera({ 0, 0 }, { 0,0 }), m_interpolate_duration(0), m_interpolate_time(0) {}
	~Camera() = default;

	Camera(const Camera& other) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other) = delete;
	bool operator==(const Camera& other) = delete;

public:
	void Update(chrono::clock::duration tick);

public:
	void LookAt(const Vector2di& position);
	void LerpTo(const Vector2di& position, const chrono::clock::duration duration);
	void FollowSceneObject(std::shared_ptr<SceneObject>& sceneobject, const chrono::clock::duration lerp_duration = 0s);

public:
	void SetFollowOffset(const Vector2di& offset);
	Vector2di GetFollowOffset() const;

public:
	void SetSize(const Vector2di& size);
	Vector2di GetSize() const;
	void SizeToWindow();

public:
	Vector2di GetPosition() const;
	const Recti& GetCamera() const;
	const Rectf GetViewRect() const;
	bool IsSizedToWindow() const;

private:
	bool m_sized_to_window = true;
	Recti m_camera;
	Vector2di m_interpolate_start;
	Vector2di m_interpolate_end;
	Vector2di m_follow_offset;
	chrono::clock::duration m_interpolate_duration;
	chrono::clock::duration m_interpolate_time;
	std::weak_ptr<SceneObject> m_follow_object;
};

inline Vector2di Camera::GetFollowOffset() const
{
	return m_follow_offset;
}

inline Vector2di Camera::GetSize() const
{
	return m_camera.size;
}

inline Vector2di Camera::GetPosition() const
{
	return m_camera.pos;
}

inline const Recti& Camera::GetCamera() const
{
	return m_camera;
}

inline const Rectf Camera::GetViewRect() const
{
	auto topleft = m_camera.pos - m_camera.size / 2;
	return Rectf(math::convert<float>(topleft), math::convert<float>(m_camera.size));
}

inline void Camera::SizeToWindow()
{
	m_sized_to_window = true;
}

inline bool Camera::IsSizedToWindow() const
{
	return m_sized_to_window;
}

} // end namespace tdrp::camera
