#pragma once

namespace tdrp
{

template <typename T, typename Flag>
constexpr T FLAGS(const Flag& flag)
{
	return static_cast<T>(flag);
}

template <typename T, typename Flag, typename... Rest>
constexpr T FLAGS(const Flag& flag, const Rest&... args)
{
	return static_cast<T>(flag) | FLAGS(args);
}

template<typename T, typename Flag>
constexpr bool HASFLAG(const T& source, const Flag& check)
{
	return source & static_cast<T>(check);
}

template<typename T, typename Flag>
constexpr void SETFLAG(T& source, const Flag& flag)
{
	source |= static_cast<T>(flag);
}

template<typename T, typename Flag>
constexpr void UNSETFLAG(T& source, const Flag& flag)
{
	source &= ~static_cast<T>(flag);
}

} // end namespace tdrp
