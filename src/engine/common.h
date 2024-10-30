#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <cstdint>
#include <cstddef>
#include <locale>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <algorithm>
#include <numeric>
#include <optional>
#include <atomic>
#include <chrono>
#include <type_traits>
#include <concepts>
#include <ranges>

#include <boost/algorithm/string.hpp>

#include "engine/events/Events.h"
#include "engine/helper/math.h"
#include "engine/helper/flags.h"
#include "engine/filesystem/Log.h"

#include <BabyDI.hpp>

namespace chrono
{
	using clock = std::chrono::steady_clock;
}
using namespace std::chrono_literals;

namespace tdrp
{

template <typename T>
concept is_numeric = std::integral<T> || std::floating_point<T>;

template <class T>
concept is_string = std::convertible_to<T, std::string>;

// Helper concept to restrict a range to a specific type.
template <typename R, typename V>
concept range_of = std::ranges::range<R> && std::same_as<std::remove_const<std::ranges::range_value_t<R>>, std::remove_const<V>>;

} // end namespace tdrp

namespace tdrp
{

using SceneObjectID = uint32_t;
using AttributeID = uint16_t;
using ItemID = uint16_t;
using PlayerID = uint16_t;

constexpr PlayerID NO_PLAYER = ~0;

} // end namespace tdrp

/*
namespace tdrp
{

class MyClass
{
public:
	MyClass() = default;
	~MyClass() = default;

	MyClass(const MyClass&) = delete;
	MyClass(MyClass&&) noexcept = delete;
	MyClass& operator=(const MyClass&) = delete;
	MyClass& operator=(MyClass&&) noexcept = delete;
	bool operator==(const MyClass&) = delete;
};

} // end namespace tdrp
*/
