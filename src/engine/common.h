#pragma once

#include <cstdint>
#include <cstddef>
#include <locale>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "engine/helper/math.h"


static inline void ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch, std::locale("")); }));
}

static inline void rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch, std::locale("")); }).base(), s.end());
}

static inline void trim(std::string& s)
{
	ltrim(s);
	rtrim(s);
}
