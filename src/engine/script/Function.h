#pragma once

#include <set>
#include <unordered_set>
#include <unordered_map>
#include <sol/sol.hpp>

#include "engine/common.h"
#include "engine/script/Script.h"


#define SCRIPT_SETUP \
public: using ValidScriptObject = std::true_type; \
sol::environment LuaEnvironment; \
std::unordered_set<tdrp::script::Function*> BoundScriptFunctions;

#define SCRIPT_ERASE do { for (auto* f : BoundScriptFunctions) f->RemoveAll(); BoundScriptFunctions.clear(); } while(false)

#define SCRIPT_FUNCTION(name) \
public: void Set##name(sol::this_state s, sol::protected_function func, sol::this_environment te) { \
	BoundScriptFunctions.insert(&name); \
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
	Function& operator=(const Function& other) = default;
	Function& operator=(Function&& other) = default;
	bool operator==(const Function& other) = delete;

public:

	template <typename ...Args>
	void Run(const std::string& module_name, Args&&... args) const
	{
		auto entry = m_function.find(module_name);
		if (entry != std::end(m_function))
		{
			if (!entry->second.valid())
			{
				m_function.erase(entry);
				return;
			}

			sol::environment env{ sol::env_key, entry->second };
			env["MODULENAME"] = entry->first;

			auto result = entry->second.call(std::forward<Args>(args)...);
			if (!result.valid())
			{
				log::PrintLine("** Lua error [MODULE {}]", entry->first);
				script::Script::ErrorHandler(entry->second.lua_state(), std::move(result));

				m_function.erase(entry);
			}
		}
	}

	template <typename ...Args>
	void RunAll(Args&&... args) const
	{
		std::set<std::string> failed;

		for (const auto& entry : m_function)
		{
			if (!entry.second.valid())
			{
				failed.insert(entry.first);
				continue;
			}

			sol::environment env{ sol::env_key, entry.second };
			env["MODULENAME"] = entry.first;

			auto result = entry.second.call(std::forward<Args>(args)...);
			if (!result.valid())
			{
				log::PrintLine("** Lua error [MODULE {}]", entry.first);
				script::Script::ErrorHandler(entry.second.lua_state(), std::move(result));

				failed.insert(entry.first);
			}
		}

		auto erase_func = [this](auto& entry) { this->m_function.erase(entry); };
		std::for_each(std::begin(failed), std::end(failed), erase_func);
	}

public:
	void Set(const std::string& module_name, sol::protected_function& function)
	{
		m_function[module_name] = std::move(function);
	}

	void Remove(const std::string& module_name)
	{
		m_function.erase(module_name);
	}

	void RemoveAll()
	{
		m_function.clear();
	}

private:
	mutable std::unordered_map<std::string, sol::protected_function> m_function;
};

} // end namespace tdrp::script
