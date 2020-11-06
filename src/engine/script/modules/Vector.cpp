#include "engine/script/modules/bind.h"

#include "engine/helper/math.h"

namespace tdrp::script::modules
{

void bind_vector(sol::state& lua)
{
	lua.new_usertype<Vector2df>("Vector2df", sol::constructors<Vector2df(), Vector2df(const Vector2df), Vector2df(float, float)>(),
		sol::meta_function::addition, sol::resolve<Vector2df(const Vector2df&, const Vector2df&)>(mathfu::operator+),
		sol::meta_function::addition, sol::resolve<Vector2df(float, const Vector2df&)>(mathfu::operator+),
		sol::meta_function::addition, sol::resolve<Vector2df(const Vector2df&, float)>(mathfu::operator+),

		sol::meta_function::subtraction, sol::resolve<Vector2df(const Vector2df&, const Vector2df&)>(mathfu::operator-),
		sol::meta_function::subtraction, sol::resolve<Vector2df(float, const Vector2df&)>(mathfu::operator-),
		sol::meta_function::subtraction, sol::resolve<Vector2df(const Vector2df&, float)>(mathfu::operator-),

		sol::meta_function::multiplication, sol::resolve<Vector2df(const Vector2df&, const Vector2df&)>(mathfu::operator*),
		sol::meta_function::multiplication, sol::resolve<Vector2df(float, const Vector2df&)>(mathfu::operator*),
		sol::meta_function::multiplication, sol::resolve<Vector2df(const Vector2df&, float)>(mathfu::operator*),

		sol::meta_function::division, sol::resolve<Vector2df(const Vector2df&, const Vector2df&)>(mathfu::operator/),
		sol::meta_function::division, sol::resolve<Vector2df(const Vector2df&, float)>(mathfu::operator/),

		sol::meta_function::equal_to, sol::resolve<bool(const Vector2df&, const Vector2df&)>(mathfu::operator==),

		sol::meta_function::unary_minus, sol::resolve<Vector2df(const Vector2df&)>(mathfu::operator-),

		sol::meta_function::index, [](Vector2df& vector, int i) -> float { if (i == 0) return vector.x; else return vector.y; },
		sol::meta_function::new_index, [](Vector2df& vector, int i, float v) -> void { if (i == 0) vector.x = v; else vector.y = v; },

		"Length", &Vector2df::Length,
		"LengthSquared", &Vector2df::LengthSquared,
		"Normalize", &Vector2df::Normalize,
		"Normalized", &Vector2df::Normalized,
		
		"X", &Vector2df::x,
		"Y", &Vector2df::y,

		"Angle", &Vector2df::Angle,
		//"CrossProduct", &Vector2df::CrossProduct,
		"Distance", &Vector2df::Distance,
		"DistanceSquared", &Vector2df::DistanceSquared,
		"DotProduct", &Vector2df::DotProduct,
		"HadamardProduct", &Vector2df::HadamardProduct,
		"Lerp", &Vector2df::Lerp,
		"Max", &Vector2df::Max,
		"Min", &Vector2df::Min,
		"RandomInRange", &Vector2df::RandomInRange
	);
}

} // end namespace tdrp::script::modules
