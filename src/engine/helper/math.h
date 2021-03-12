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


template<typename T>
concept is_vector3 =
requires (T& t)
{
	{ t.x };
	{ t.y };
	{ t.z };
};

template<typename T>
concept vector_pack_2 = (sizeof T::data_ / sizeof(std::remove_all_extents_t<decltype(T::x)>)) == 2;

template<typename T>
concept is_vector2 =
requires (T& t)
{
	{ t.x };
	{ t.y };
	requires vector_pack_2<T>;
};

template<typename T, typename U> requires is_vector2<T> && is_vector2<U>
inline T VectorConvert(const U& other)
{
	return T(static_cast<decltype(T::x)>(other.x), static_cast<decltype(T::y)>(other.y));
}

template<typename T, typename U> requires is_vector3<T> && is_vector3<U>
inline T VectorConvert(const U& other)
{
	return T(static_cast<decltype(T::x)>(other.x), static_cast<decltype(T::y)>(other.y), static_cast<decltype(T::z)>(other.z));
}


inline Vector2df operator+(const Vector2df& lhs, const Vector2di& rhs)
{
	return lhs + VectorConvert<Vector2df>(rhs);
}
inline Vector2di operator+(const Vector2di& lhs, const Vector2df& rhs)
{
	return lhs + VectorConvert<Vector2di>(rhs);
}

inline Vector2df operator-(const Vector2df& lhs, const Vector2di& rhs)
{
	return lhs - VectorConvert<Vector2df>(rhs);
}
inline Vector2di operator-(const Vector2di& lhs, const Vector2df& rhs)
{
	return lhs - VectorConvert<Vector2di>(rhs);
}

} // end namespace tdrp

namespace tdrp::math
{

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
