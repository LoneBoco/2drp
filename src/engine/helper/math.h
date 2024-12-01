#pragma once

#include <concepts>
#include <mathfu/vector.h>
#include <mathfu/rect.h>
#include <tmxlite/Types.hpp>

namespace tdrp
{

typedef mathfu::Vector<float, 2> Vector2df;
typedef mathfu::Vector<float, 3> Vector3df;
typedef mathfu::Vector<int32_t, 2> Vector2di;
typedef mathfu::Vector<int32_t, 3> Vector3di;
typedef mathfu::Vector<uint32_t, 2> Vector2du;

typedef mathfu::Rect<float> Rectf;
typedef mathfu::Rect<int32_t> Recti;

} // end namespace tdrp

namespace tdrp::math
{

template <typename T>
concept Coordinate2D = requires(T a)
{
	{ a.x };
	{ a.y };
};

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

inline constexpr auto next_power_of_two(std::integral auto value) -> decltype(value)
{
	if (value == 0) return 2;

	--value;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	
	if constexpr (sizeof(value) > 1)
	{
		value |= value >> 8;
	}

	if constexpr (sizeof(value) > 2)
	{
		value |= value >> 16;
	}

	if constexpr (sizeof(value) > 4)
	{
		value |= value >> 32;
	}

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

/// <summary>
/// Checks if the check rectangle is contained within or intersects with the against rectangle.
/// </summary>
/// <typeparam name="T">The data type of the rectangle.</typeparam>
/// <param name="check">The rectangle that could be contained.</param>
/// <param name="against">The rectangle that could be doing the containing.</param>
/// <returns>True if the check rectangle is contained within or intersects the against rectangle.</returns>
template <typename T>
inline bool containsOrIntersects(const mathfu::Rect<T>& check, const mathfu::Rect<T>& against)
{
	return (check.pos.x + check.size.x) >= against.pos.x
		&& (check.pos.y + check.size.y) >= against.pos.y
		&& check.pos.x <= (against.pos.x + against.size.x)
		&& check.pos.y <= (against.pos.y + against.size.y)
		;
}

/// <summary>
/// Checks if the check rectangle is contained within or intersects with the against rectangle.
/// </summary>
/// <typeparam name="T">The data type of the rectangle.</typeparam>
/// <typeparam name="V">A class that contains an x/y member.</typeparam>
/// <param name="check">The rectangle that could be contained.</param>
/// <param name="against_pos">The top-left position of the rectangle that could be contained.</param>
/// <param name="against_size">The dimensions of the rectangle that could be contained.</param>
/// <returns>True if the check rectangle is contained within or intersects the against rectangle.</returns>
template <typename T, Coordinate2D V, Coordinate2D U>
inline bool containsOrIntersects(const mathfu::Rect<T>& check, const V& against_pos, const U& against_size)
{
	return (check.pos.x + check.size.x) >= against_pos.x
		&& (check.pos.y + check.size.y) >= against_pos.y
		&& check.pos.x <= (against_pos.x + against_size.x)
		&& check.pos.y <= (against_pos.y + against_size.y)
		;
}

/// <summary>
/// Checks if the check rectangle is contained within or intersects with the against rectangle.
/// </summary>
/// <typeparam name="T">The data type of the rectangle.</typeparam>
/// <typeparam name="V">A class that contains an x/y member.</typeparam>
/// <param name="check_pos">The top-left of the rectangle that could be contained.</param>
/// <param name="check_size">The dimensions of the rectangle that could be contained.</param>
/// <param name="against">The rectangle that could be doing the containing.</param>
/// <returns>True if the check rectangle is contained within or intersects the against rectangle.</returns>
template <typename T, Coordinate2D V, Coordinate2D U>
inline bool containsOrIntersects(const V& check_pos, const U& check_size, const mathfu::Rect<T>& against)
{
	return (check_pos.x + check_size.x) >= against.pos.x
		&& (check_pos.y + check_size.y) >= against.pos.y
		&& check_pos.x <= (against.pos.x + against.size.x)
		&& check_pos.y <= (against.pos.y + against.size.y)
		;
}

template <typename T>
inline bool containsOrIntersects(const mathfu::Rect<T>& check, const mathfu::Vector<T, 2>& against_circle_origin, float against_circle_radius)
{
	mathfu::Vector<T, 2> center_rect{ check.pos.x + (check.size.x / 2), check.pos.y + (check.size.y / 2) };
	
	mathfu::Vector<double, 2> circle_distance;
	circle_distance.x = std::abs(against_circle_origin.x - center_rect.x);
	circle_distance.y = std::abs(against_circle_origin.y - center_rect.y);

	if (circle_distance.x > (check.size.x / 2 + against_circle_radius))
		return false;
	if (circle_distance.y > (check.size.y / 2 + against_circle_radius))
		return false;

	if (circle_distance.x <= (check.size.x / 2))
		return true;
	if (circle_distance.y <= (check.size.y / 2))
		return true;

	T circle_distance_sq = static_cast<T>(std::pow(circle_distance.x - check.size.x / 2, 2) + std::pow(circle_distance.y - check.size.y / 2, 2));
	return (circle_distance_sq <= std::pow(against_circle_radius, 2));
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

template <std::signed_integral T, std::unsigned_integral U>
inline mathfu::Vector<T, 2> operator*(const mathfu::Vector<T, 2>& lhs, const mathfu::Vector<U, 2>& rhs)
{
	return mathfu::Vector<T, 2>{ lhs.x * static_cast<T>(rhs.x), lhs.y * static_cast<T>(rhs.y) };
}

inline tmx::Vector2i operator*(const tmx::Vector2i& lhs, const tmx::Vector2u& rhs)
{
	return tmx::Vector2i(lhs.x * rhs.x, lhs.y * rhs.y);
}

} // end namespace tdrp
