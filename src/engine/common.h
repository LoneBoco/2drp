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
#include <functional>
#include <algorithm>
#include <numeric>
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

} // end namespace tdrp

namespace tdrp
{

using SceneObjectID = uint32_t;
using AttributeID = uint16_t;

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
	MyClass(MyClass&&) = delete;
	MyClass& operator=(const MyClass&) = delete;
	MyClass& operator=(MyClass&&) = delete;
	bool operator==(const MyClass&) = delete;
};

} // end namespace tdrp
*/
