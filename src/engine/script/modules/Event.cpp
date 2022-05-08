#include "engine/script/modules/bind.h"

#include "engine/scene/SceneObject.h"


namespace tdrp::script::modules
{

void bind_events(sol::state& lua)
{
	lua.new_usertype<EventHandleBase>("EventHandleBase", sol::no_constructor);
}

} // end namespace tdrp::script::modules
