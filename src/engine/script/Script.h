#pragma once

#include <string_view>
#include <unordered_map>
#include <initializer_list>

#include <sol/sol.hpp>

#include "engine/common.h"


namespace tdrp::script
{

template <typename T>
concept ValidScriptObject = requires (T t)
{
	T::ValidScriptObject;
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
	sol::state& GetLuaState() { return lua; }
	const sol::state& GetLuaState() const { return lua; }

public:
	static sol::protected_function_result ErrorHandler(lua_State*, sol::protected_function_result pfr)
	{
		if (!pfr.valid())
		{
			sol::error err = pfr;
			log::PrintLine("[LUA][ERROR] {}", err.what());
		}
		return pfr;
	}
	static int DefaultErrorHandler(lua_State* state, sol::optional<const std::exception&> ex, sol::string_view what)
	{
		log::PrintLine("[LUA][ERROR] An exception has occured: {}", what);
		lua_pushlstring(state, what.data(), what.size());
		return 1;
	}

public:
	template <typename T> requires ValidScriptObject<T>
	void RunScript(const std::string& module_name, const std::string& script, std::shared_ptr<T> me)
	{
		if (script.empty())
			return;

		me->LuaEnvironment = sol::environment(lua, sol::create, lua.globals());
		me->LuaEnvironment["MODULENAME"] = module_name;
		me->LuaEnvironment["Me"] = me;
		lua.safe_script(script, me->LuaEnvironment, Script::ErrorHandler);
	}

	template <typename T> requires ValidScriptObject<T>
	void RunScript(const std::string& module_name, const std::string& script, T* me)
	{
		if (script.empty())
			return;

		me->LuaEnvironment = sol::environment(lua, sol::create, lua.globals());
		me->LuaEnvironment["MODULENAME"] = module_name;
		me->LuaEnvironment["Me"] = me;
		lua.safe_script(script, me->LuaEnvironment, Script::ErrorHandler);
	}

public:
	template <class F>
		requires std::invocable<F, sol::state&>
	void BindIntoMe(F function)
	{
		std::invoke(function, lua);
	}
	template <class F, typename... Args>
		requires std::invocable<F, sol::state&>
	void BindIntoMe(F function, Args... args)
	{
		std::invoke(function, lua);
		return BindIntoMe(args...);
	}

	template <class F>
		requires std::invocable<F, sol::state*>
	void BindIntoMe(F function)
	{
		std::invoke(function, &lua);
	}
	template <class F, typename... Args>
		requires std::invocable<F, sol::state*>
	void BindIntoMe(F function, Args... args)
	{
		std::invoke(function, &lua);
		return BindIntoMe(args...);
	}

protected:
	sol::state lua;
};

using ScriptPtr = std::shared_ptr<Script>;


class ScriptManager
{
public:
	ScriptManager() = default;
	~ScriptManager();

	ScriptManager(const ScriptManager& other) = delete;
	ScriptManager(ScriptManager&& other) = delete;
	ScriptManager& operator=(const ScriptManager& other) = delete;
	ScriptManager& operator=(ScriptManager&& other) = delete;
	bool operator==(const ScriptManager& other) = delete;

public:
	std::shared_ptr<Script> CreateScriptInstance(const std::string& name);
	std::shared_ptr<Script> GetScriptInstance(const std::string& name);

protected:
	std::unordered_map<std::string, std::shared_ptr<Script>> m_script_instances;
};

} // end namespace tdrp::script
