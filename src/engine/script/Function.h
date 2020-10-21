#pragma once

#include <unordered_map>
#include <sol/sol.hpp>

#include "engine/common.h"

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
	template <typename ...Args>
	void Run(const std::string& module_name, Args&&... args)
	{
		auto entry = m_function.find(module_name);
		if (entry != std::end(m_function))
		{
			auto result = entry->second.call(std::forward<Args>(args)...);
			script::Script::ErrorHandler(entry->second.lua_state(), std::move(result));
		}
	}

	template <typename ...Args>
	void RunAll(Args&&... args) const
	{
		for (const auto& entry : m_function)
		{
			auto result = entry.second.call(std::forward<Args>(args)...);
			script::Script::ErrorHandler(entry.second.lua_state(), std::move(result));
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
