#include "client/game/Camera.h"

namespace tdrp::camera
{

void Camera::Update(chrono::clock::duration tick)
{
	if (m_interpolate_duration != chrono::clock::duration::zero())
	{
		m_interpolate_time += tick;
		if (m_interpolate_time > m_interpolate_duration)
			m_interpolate_time = m_interpolate_duration;

		// Can't use Vector2di::Lerp because mathfu sucks.
		double percent = static_cast<double>(m_interpolate_duration.count()) / static_cast<double>(m_interpolate_time.count());
		double one_minus_percent = 1.0 - percent;

		m_camera.pos.x = static_cast<int32_t>(one_minus_percent * m_interpolate_start.x) + static_cast<int32_t>(percent * m_interpolate_end.x);
		m_camera.pos.y = static_cast<int32_t>(one_minus_percent * m_interpolate_start.y) + static_cast<int32_t>(percent * m_interpolate_end.y);

		// If we reached the end, stop the interpolation.
		if (m_interpolate_time == m_interpolate_duration)
			m_interpolate_duration = chrono::clock::duration::zero();
	}
}

void Camera::LookAt(const Vector2di& position)
{
	m_camera.pos = position;
}

void Camera::LerpTo(const Vector2di& position, const chrono::clock::duration duration)
{
	m_interpolate_start = m_camera.pos;
	m_interpolate_end = position;
	m_interpolate_duration = duration;
	m_interpolate_time = chrono::clock::duration::zero();
}

void Camera::SetSize(const Vector2di& size)
{
	m_camera.size = size;
}

} // end namespace tdrp::camera
