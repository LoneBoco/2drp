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
