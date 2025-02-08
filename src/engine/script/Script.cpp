#include "engine/filesystem/Log.h"
#include "engine/script/Script.h"
#include "engine/script/modules/bind.h"

namespace tdrp::script
{

std::unordered_map<std::string, std::string> Script::scripts{};

static void log(const char* message)
{
	log::PrintLine(log::game, "[LUA] {}", message);
}

sol::protected_function_result Script::ErrorHandler(lua_State* state, sol::protected_function_result pfr, std::string_view module_name)
{
	if (!pfr.valid())
	{
		sol::error err = pfr;

		std::string error{ err.what() };

		// Try to fix up the error string by using the actual lines of code the error occurred on.
		if (auto script_iter = scripts.find(std::string{ module_name }); script_iter != scripts.end())
		{
			std::string& script = script_iter->second;
			size_t beginning = std::string::npos;
			while ((beginning = error.find("[string")) != std::string::npos)
			{
				auto end = error.find("]:", beginning);
				if (end == std::string::npos)
					break;
				auto line_start = end + 2;
				auto line_end = error.find(":", line_start);
				if (line_end == std::string::npos)
					line_end = error.find(">", line_start);
				if (line_end == std::string::npos)
					break;

				try
				{
					auto line_number = std::stoi(error.substr(line_start, line_end - line_start));
					auto r = script | std::ranges::views::split('\n') | std::ranges::views::drop(line_number - 1) | std::ranges::views::take(1);
					if (r.empty())
						break;

					std::span<char> split_range = r.front();
					std::string line{ split_range.begin(), split_range.end() };
					boost::trim(line);

					error.replace(error.begin() + beginning + 1, error.begin() + end, line);
					beginning = end + 1;
				}
				catch (...)
				{
					break;
				}
			}
		}

		log::PrintLine(log::game, "[LUA][ERROR][MODULE {}] {}", module_name, error);
	}
	return pfr;
}

int Script::DefaultExceptionHandler(lua_State* state, sol::optional<const std::exception&> ex, sol::string_view what)
{
	log::PrintLine(log::game, "[LUA][ERROR] An exception has occured: {}", what);
	lua_pushlstring(state, what.data(), what.size());
	return 1;
}


Script::Script()
{
	lua = std::make_unique<sol::state>();

	lua->open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math);
	sol::set_default_exception_handler(*lua, &DefaultExceptionHandler);

	script::modules::bind_events(*lua);
	script::modules::bind_item(*lua);
	script::modules::bind_player(*lua);
	script::modules::bind_scene(*lua);
	script::modules::bind_attributes(*lua);
	script::modules::bind_sceneobject(*lua);
	script::modules::bind_server(*lua);
	script::modules::bind_vector(*lua);
	script::modules::bind_physics(*lua);

	lua->set_function("log", &log);
}

Script::~Script()
{
	lua->collect_garbage();
	lua->stack_clear();

	environments.clear();
	lua.reset();
}

ScriptManager::~ScriptManager()
{
	log::PrintLine(log::game, ":: Shutting down scripting system.");
	try
	{
		// An object gets deleted somewhere and Lua can't handle it.
		// We are terminating anyway so just keep going.
		m_script_instances.clear();
	}
	catch (...)
	{}
}

std::shared_ptr<Script> ScriptManager::CreateScriptInstance(const std::string& name)
{
	auto it = m_script_instances.find(name);
	if (it != std::end(m_script_instances))
		return it->second;

	auto script = std::make_shared<Script>();
	m_script_instances.insert(std::make_pair(name, script));
	return script;
}

std::shared_ptr<Script> ScriptManager::GetScriptInstance(const std::string& name)
{
	auto it = m_script_instances.find(name);
	if (it == std::end(m_script_instances))
		return nullptr;

	return it->second;
}

bool ScriptManager::EraseScriptInstance(const std::string& name)
{
	auto it = m_script_instances.find(name);
	if (it == std::end(m_script_instances))
		return false;

	m_script_instances.erase(it);
	return true;
}

} // end namespace tdrp::script
