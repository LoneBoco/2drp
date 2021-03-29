#include "engine/script/Script.h"
#include "engine/script/modules/bind.h"

namespace tdrp::script
{

void log(const char* message)
{
	std::cout << message << std::endl;
}

Script::Script()
{
	lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::jit);
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
