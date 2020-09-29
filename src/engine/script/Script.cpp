#include "engine/script/Script.h"

namespace tdrp::script
{

Script::Script()
{
	lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::jit);
}

Script::~Script()
{

}

void Script::RunScript(const std::string& name, tdrp::SceneObject& so, const std::string_view& script)
{
	lua["MODULENAME"] = name;

	lua["Me"] = std::shared_ptr<tdrp::SceneObject>(&so);
	lua.safe_script(script);
	lua["Me"] = nullptr;
}

} // end namespace tdrp::script
