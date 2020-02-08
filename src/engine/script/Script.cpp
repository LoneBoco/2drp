#include "engine/script/Script.h"

namespace tdrp::script
{

Script::Script()
{
	lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math);
}

Script::~Script()
{

}

} // end namespace tdrp::script
