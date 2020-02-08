#include "engine/script/modules/Vector.h"

#include "engine/helper/math.h"

namespace tdrp::script::modules
{

void bind_vector(sol::state& lua)
{
	lua.new_usertype<Vector2df>("vector2df");
}

} // end namespace tdrp::script::modules
