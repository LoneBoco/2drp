#include "engine/script/Script.h"
#include "engine/script/modules/bind.h"

namespace tdrp::script
{

void log(const char* message)
{
	log::PrintLine("[LUA] {}", message);
}

Script::Script()
{
	lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::jit);
	sol::set_default_exception_handler(lua, &DefaultErrorHandler);

	script::modules::bind_events(lua);
	script::modules::bind_player(lua);
	script::modules::bind_scene(lua);
	script::modules::bind_attributes(lua);
	script::modules::bind_sceneobject(lua);
	script::modules::bind_server(lua);
	script::modules::bind_vector(lua);

	lua.set_function("log", &log);
}

Script::~Script()
{
}

ScriptManager::~ScriptManager()
{
	log::PrintLine(":: Shutting down scripting system.");
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

} // end namespace tdrp::script
