#pragma once

#include <memory>

#include <sol/sol.hpp>

#include "engine/events/Events.h"
#include "engine/scene/ObjectAttributes.h"


namespace tdrp::script::helpers
{

EventHandle subscribe(Attribute& self, sol::protected_function func);
sol::object asObject(const AttributePtr& attribute, sol::this_state s);
void fromObject(Attribute& attribute, const sol::object& object);

} // end namespace tdrp::script::helpers
