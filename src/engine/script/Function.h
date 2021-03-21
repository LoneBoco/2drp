#pragma once

#include <unordered_map>
#include <sol/sol.hpp>

#include "engine/common.h"
#include "engine/script/Script.h"


#define SCRIPT_ENVIRONMENT \
public: sol::environment LuaEnvironment;

#define SCRIPT_FUNCTION(name) \
public: void Set##name(sol::this_state s, sol::protected_function func, sol::this_environment te) { \
	sol::environment& env = te; \
	if (env) { \
		std::string mod = env["MODULENAME"]; \
		name.Set(mod, func); \
	} else { \
		sol::state_view lua{ s }; \
		std::string mod = lua["MODULENAME"]; \
		name.Set(mod, func); \
	} \
} \
tdrp::script::Function name;


namespace tdrp::script
{

class Function
{
public:
	Function() = default;
	~Function() = default;

	Function(const Function& other) = delete;
	Function(Function&& other) = delete;
	Function& operator=(const Function& other) = delete;
	Function& operator=(Function&& other) = delete;
	bool operator==(const Function& other) = delete;

public:

	template <typename T, typename ...Args> requires ScriptWithEnvironment<T>
	void Run(const std::string& module_name, Args&&... args)
	{
		auto entry = m_function.find(module_name);
		if (entry != std::end(m_function))
		{
			sol::environment env{ sol::env_key, entry->second };
			env["MODULENAME"] = entry->first;

			auto result = entry->second.call(std::forward<Args>(args)...);
			script::Script::ErrorHandler(entry->second.lua_state(), std::move(result));
		}
	}

	template <typename T, typename ...Args>
	void Run(T& me, const std::string& module_name, Args&&... args)
	{
		auto entry = m_function.find(module_name);
		if (entry != std::end(m_function))
		{
			if constexpr (ScriptWithEnvironment<T>)
			{
				me.LuaEnvironment["MODULENAME"] = module_name;
				me.LuaEnvironment["Me"] = &me;

				auto result = entry->second.call(std::forward<Args>(args)...);
				script::Script::ErrorHandler(entry->second.lua_state(), std::move(result));
			}
			else
			{
				sol::state_view s{ entry->second.lua_state() };
				s["MODULENAME"] = module_name;
				s["Me"] = &me;

				auto result = entry->second.call(std::forward<Args>(args)...);
				script::Script::ErrorHandler(entry->second.lua_state(), std::move(result));

				s["Me"] = nullptr;
			}
		}
	}

	template <typename T, typename ...Args> requires ScriptWithEnvironment<T>
	void RunAll(Args&&... args) const
	{
		for (const auto& entry : m_function)
		{
			sol::environment env{ sol::env_key, entry.second };
			env["MODULENAME"] = entry.first;

			auto result = entry.second.call(std::forward<Args>(args)...);
			script::Script::ErrorHandler(entry.second.lua_state(), std::move(result));
		}
	}

	template <typename T, typename ...Args>
	void RunAll(T& me, Args&&... args) const
	{
		for (const auto& entry : m_function)
		{
			if constexpr (ScriptWithEnvironment<T>)
			{
				me.LuaEnvironment["MODULENAME"] = entry.first;
				me.LuaEnvironment["Me"] = &me;

				auto result = entry.second.call(std::forward<Args>(args)...);
				script::Script::ErrorHandler(entry.second.lua_state(), std::move(result));
			}
			else
			{
				sol::state_view s{ entry.second.lua_state() };
				s["MODULENAME"] = entry.first;
				s["Me"] = &me;

				auto result = entry.second.call(std::forward<Args>(args)...);
				script::Script::ErrorHandler(entry.second.lua_state(), std::move(result));

				s["Me"] = nullptr;
			}
		}
	}

public:
	void Set(const std::string& module_name, sol::protected_function& function)
	{
		m_function[module_name] = function;
	}

	void Remove(const std::string& module_name)
	{
		m_function.erase(module_name);
	}

private:
	std::unordered_map<std::string, sol::protected_function> m_function;
};

} // end namespace tdrp::script
