#pragma once

#include <string_view>
#include <unordered_map>

#include <sol/sol.hpp>

#include "engine/common.h"


namespace tdrp::script
{

template <typename T>
concept ScriptWithEnvironment =
	requires (T& t) {
		t.LuaEnvironment;
};

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
	static sol::protected_function_result ErrorHandler(lua_State*, sol::protected_function_result pfr)
	{
		if (!pfr.valid())
		{
			sol::error err = pfr;
			std::cout << "[LUA ERROR] " << err.what() << std::endl;
		}
		return pfr;
	}

public:
	template <typename T>
	void RunScript(const std::string& module_name, const std::string_view& script, T& me);

	template <typename T>
	void RunScript(const std::string& module_name, const std::string_view& script, std::shared_ptr<T>& me);

protected:
	sol::state lua;
};

template <typename T>
inline void Script::RunScript(const std::string& module_name, const std::string_view& script, T& me)
{
	if (script.empty())
		return;

	if constexpr (ScriptWithEnvironment<T>)
	{
		me.LuaEnvironment = sol::environment(lua, sol::create, lua.globals());
		me.LuaEnvironment["MODULENAME"] = module_name;
		me.LuaEnvironment["Me"] = &me;

		lua.safe_script(script, me.LuaEnvironment, Script::ErrorHandler);

		// me.LuaEnvironment["Me"] = nullptr;
	}
	else
	{
		lua["MODULENAME"] = module_name;
		lua["Me"] = &me;

		lua.safe_script(script, Script::ErrorHandler);

		lua["Me"] = nullptr;
	}
}

template <typename T>
inline void Script::RunScript(const std::string& module_name, const std::string_view& script, std::shared_ptr<T>& me)
{
	if (script.empty())
		return;

	if constexpr (ScriptWithEnvironment<T>)
	{
		me->LuaEnvironment = sol::environment(lua, sol::create, lua.globals());
		me->LuaEnvironment["MODULENAME"] = module_name;
		me->LuaEnvironment["Me"] = std::shared_ptr<T>(me);

		lua.safe_script(script, me->LuaEnvironment, Script::ErrorHandler);

		// me->LuaEnvironment["Me"] = nullptr;
	}
	else
	{
		lua["MODULENAME"] = module_name;
		lua["Me"] = std::shared_ptr<T>(me);

		lua.safe_script(script, Script::ErrorHandler);

		lua["Me"] = nullptr;
	}
}

} // end namespace tdrp::script
