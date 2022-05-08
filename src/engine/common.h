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
//#include <ranges>

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

/*
namespace tdrp
{

class MyClass
{
public:
	MyClass() = default;
	~MyClass() = default;

	MyClass(const MyClass& other) = delete;
	MyClass(MyClass&& other) = delete;
	MyClass& operator=(const MyClass& other) = delete;
	MyClass& operator=(MyClass&& other) = delete;
	bool operator==(const MyClass& other) = delete;
};

} // end namespace tdrp
*/
