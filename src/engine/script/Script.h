#pragma once

#include "engine/common.h"

#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <initializer_list>

#include <sol/sol.hpp>

namespace tdrp::script
{

template <typename T> struct decay_smart { typedef T type; };
template <typename T> struct decay_smart<T*> { typedef T type; };
template <typename T> struct decay_smart<std::shared_ptr<T>> { typedef T type; };

template <typename T>
concept IsValidScriptObject = requires (T t)
{
	typename decay_smart<T>::type::ValidScriptObject;
	t->LuaEnvironment;
	*t;
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
	sol::state& GetLuaState() { return *lua; }
	const sol::state& GetLuaState() const { return *lua; }
	void CollectGarbage() { if (lua) lua->collect_garbage(); }

public:
	static sol::protected_function_result ErrorHandler(lua_State* state, sol::protected_function_result pfr, std::string_view module_name);
	static int DefaultExceptionHandler(lua_State* state, sol::optional<const std::exception&> ex, sol::string_view what);

public:

	void RunScript(std::string_view module_name, const std::string& script, script::IsValidScriptObject auto me)
	{
		if (script.empty())
			return;

		scripts.insert(std::make_pair(std::string{ module_name }, script));

		me->LuaEnvironment = std::make_shared<sol::environment>(*lua, sol::create, lua->globals());
		environments.insert(me->LuaEnvironment);

		(*me->LuaEnvironment)["MODULENAME"] = module_name;
		(*me->LuaEnvironment)["Me"] = me;
		sol::protected_function_result pfr = lua->safe_script(script, *me->LuaEnvironment);
		if (!pfr.valid())
			ErrorHandler(pfr.lua_state(), std::move(pfr), module_name);
	}

public:
	template <class F>
		requires std::invocable<F, sol::state&>
	void BindIntoMe(F function)
	{
		std::invoke(function, *lua);
	}
	template <class F, typename... Args>
		requires std::invocable<F, sol::state&>
	void BindIntoMe(F function, Args... args)
	{
		std::invoke(function, *lua);
		return BindIntoMe(args...);
	}

	template <class F>
		requires std::invocable<F, sol::state*>
	void BindIntoMe(F function)
	{
		std::invoke(function, lua->get());
	}
	template <class F, typename... Args>
		requires std::invocable<F, sol::state*>
	void BindIntoMe(F function, Args... args)
	{
		std::invoke(function, lua->get());
		return BindIntoMe(args...);
	}

protected:
	std::unique_ptr<sol::state> lua;
	std::unordered_set<std::shared_ptr<sol::environment>> environments;
	static std::unordered_map<std::string, std::string> scripts;
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
	bool EraseScriptInstance(const std::string& name);

protected:
	std::unordered_map<std::string, std::shared_ptr<Script>> m_script_instances;
};

} // end namespace tdrp::script
