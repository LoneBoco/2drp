#pragma once

#include <sol/sol.hpp>

#include "engine/common.h"

namespace tdrp::fs
{
	class File;
}

namespace tdrp::script
{

class Script
{
public:
	Script();
	~Script();

	Script(const Script& other) = delete;
	Script(Script&& other) = delete;
	Script& operator=(const Script& other) = delete;
	Script& operator=(Script&& other) = delete;
	bool operator==(const Script& other) = delete;

public:
	sol::state& GetLuaState()
	{
		return lua;
	}

	const sol::state& GetLuaState() const
	{
		return lua;
	}

public:
	void RunScript(const char* script);
	void RunScript(const std::string& script);
	void RunScript(const tdrp::fs::File& file);

protected:
	sol::state lua;
};

} // end namespace tdrp::script