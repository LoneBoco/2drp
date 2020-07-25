#pragma once

#define WIN32_LEAN_AND_MEAN

#include <cstdint>
#include <cstddef>
#include <locale>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <functional>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "engine/helper/math.h"

#include "BabyDI.hpp"

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
