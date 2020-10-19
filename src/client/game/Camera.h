#pragma once

#include "engine/common.h"
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
	void SetSize(const Vector2di& size);
	void SizeToWindow();

public:
	const Recti& GetViewWindow() const;
	bool IsSizedToWindow() const;

private:
	bool m_sized_to_window = true;
	Recti m_camera;
	Vector2di m_interpolate_start;
	Vector2di m_interpolate_end;
	chrono::clock::duration m_interpolate_duration;
	chrono::clock::duration m_interpolate_time;
};

inline const Recti& Camera::GetViewWindow() const
{
	return m_camera;
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
