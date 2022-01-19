#include <SFML/Audio/Listener.hpp>

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
		double percent = static_cast<double>(m_interpolate_time.count()) / static_cast<double>(m_interpolate_duration.count());
		double one_minus_percent = 1.0 - percent;

		// If we are following a scene object, we need to adjust the end point.
		if (auto so = m_follow_object.lock())
		{
			m_interpolate_end = math::convert<int32_t>(so->GetPosition());
		}

		m_camera.pos.x = static_cast<int32_t>(one_minus_percent * m_interpolate_start.x) + static_cast<int32_t>(percent * m_interpolate_end.x);
		m_camera.pos.y = static_cast<int32_t>(one_minus_percent * m_interpolate_start.y) + static_cast<int32_t>(percent * m_interpolate_end.y);

		// If we reached the end, stop the interpolation.
		if (m_interpolate_time == m_interpolate_duration)
			m_interpolate_duration = chrono::clock::duration::zero();
	}
	else if (!m_follow_object.expired())
	{
		if (auto so = m_follow_object.lock())
		{
			m_camera.pos = math::convert<int32_t>(so->GetPosition()) + m_follow_offset;
		}
	}

	sf::Listener::setDirection({ 0.0f, 1.0f, 0.0f });
	sf::Listener::setPosition({ static_cast<float>(m_camera.pos.x), 0.0f, static_cast<float>(m_camera.pos.y) });
}

void Camera::LookAt(const Vector2di& position)
{
	m_follow_object.reset();
	m_camera.pos = position;
}

void Camera::LerpTo(const Vector2di& position, const chrono::clock::duration duration)
{
	m_follow_object.reset();
	m_interpolate_start = m_camera.pos;
	m_interpolate_end = position;
	m_interpolate_duration = duration;
	m_interpolate_time = chrono::clock::duration::zero();
}

void Camera::FollowSceneObject(std::shared_ptr<SceneObject>& sceneobject, const chrono::clock::duration lerp_duration)
{
	m_follow_object = sceneobject;
	if (lerp_duration != 0s)
	{
		m_interpolate_start = m_camera.pos;
		// m_interpolate_end;
		m_interpolate_duration = lerp_duration;
		m_interpolate_time = chrono::clock::duration::zero();
	}
}

void Camera::SetFollowOffset(const Vector2di& offset)
{
	m_follow_offset = offset;
}

void Camera::SetSize(const Vector2di& size)
{
	m_camera.size = size;
}

} // end namespace tdrp::camera
