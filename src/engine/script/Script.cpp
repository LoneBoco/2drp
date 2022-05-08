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

} // end namespace tdrp::script
