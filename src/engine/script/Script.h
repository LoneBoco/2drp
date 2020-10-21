#pragma once

#include <string_view>
#include <unordered_map>

#include <sol/sol.hpp>

#include "engine/common.h"
#include "engine/script/Function.h"


#define SCRIPT_FUNCTION(name) \
public: void Set##name(sol::this_state s, sol::protected_function func) { \
	sol::state_view lua{ s }; \
	std::string mod = lua["MODULENAME"]; \
	name.Set(mod, func); \
} \
tdrp::script::Function name;


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
	lua["MODULENAME"] = module_name;

	lua["Me"] = &me;
	lua.safe_script(script, Script::ErrorHandler);

	lua["Me"] = nullptr;
}

template <typename T>
inline void Script::RunScript(const std::string& module_name, const std::string_view& script, std::shared_ptr<T>& me)
{
	lua["MODULENAME"] = module_name;

	lua["Me"] = std::shared_ptr<T>(me);
	lua.safe_script(script, Script::ErrorHandler);
	
	lua["Me"] = nullptr;
}

} // end namespace tdrp::script
