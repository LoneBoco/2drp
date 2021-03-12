#pragma once

#include <mathfu/vector.h>
#include <mathfu/rect.h>

namespace tdrp
{

typedef mathfu::Vector<float, 2> Vector2df;
typedef mathfu::Vector<float, 3> Vector3df;
typedef mathfu::Vector<int32_t, 2> Vector2di;
typedef mathfu::Vector<int32_t, 3> Vector3di;

typedef mathfu::Rect<float> Rectf;
typedef mathfu::Rect<int32_t> Recti;

} // end namespace tdrp

namespace tdrp::math
{

template <typename T, typename U>
inline mathfu::Vector<T, 2> convert(const mathfu::Vector<U, 2>& other)
{
	return mathfu::Vector<T, 2>(static_cast<T>(other.x), static_cast<T>(other.y));
}

template <typename T, typename U>
inline mathfu::Vector<T, 3> convert(const mathfu::Vector<U, 3>& other)
{
	return mathfu::Vector<T, 3>(static_cast<T>(other.x), static_cast<T>(other.y), static_cast<T>(other.z));
}

template <typename T, typename U>
inline mathfu::Rect<T> convert(const mathfu::Rect<U>& other)
{
	return mathfu::Rect<T>(static_cast<T>(other.pos.x), static_cast<T>(other.pos.y), static_cast<T>(other.size.x), static_cast<T>(other.size.y));
}

inline uint32_t next_power_of_two(uint32_t value)
{
	if (value == 0) return 2;

	--value;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	return ++value;
}

inline int32_t next_power_of_two(int32_t value)
{
	if (value == 0) return 2;

	--value;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	return ++value;
}

template <typename T>
inline bool contains(const mathfu::Rect<T>& check, const mathfu::Rect<T>& against)
{
	return check.pos.x >= against.pos.x
		&& check.pos.y >= against.pos.y
		&& (check.pos.x + check.size.x) <= (against.pos.x + against.size.x)
		&& (check.pos.y + check.size.y) <= (against.pos.y + against.size.y)
		;
}

template <typename T, typename U>
inline bool contains(const mathfu::Vector<T, 2>& check, const mathfu::Rect<U>& against)
{
	return check.x >= against.pos.x
		&& check.y >= against.pos.y
		&& check.x <= (against.pos.x + against.size.x)
		&& check.y <= (against.pos.y + against.size.y)
		;
}

template <typename T>
inline bool containsOrIntersects(const mathfu::Rect<T>& check, const mathfu::Rect<T>& against)
{
	return (check.pos.x + check.size.x) >= against.pos.x
		&& (check.pos.y + check.size.y) >= against.pos.y
		&& check.pos.x <= (against.pos.x + against.size.x)
		&& check.pos.y <= (against.pos.y + against.size.y)
		;
}

} // end namespace tdrp::math

namespace tdrp
{

template <typename T, typename U, typename = std::enable_if_t<!std::is_same_v<T, U>, bool>>
inline mathfu::Vector<T, 2> operator+(const mathfu::Vector<T, 2>& lhs, const mathfu::Vector<U, 2>& rhs)
{
	return lhs + math::convert<T>(rhs);
}

template <typename T, typename U, typename = std::enable_if_t<!std::is_same_v<T, U>, bool>>
inline mathfu::Vector<T, 2> operator-(const mathfu::Vector<T, 2>& lhs, const mathfu::Vector<U, 2>& rhs)
{
	return lhs - math::convert<T>(rhs);
}

} // end namespace tdrp
