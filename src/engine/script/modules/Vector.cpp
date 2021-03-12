#include "engine/script/modules/bind.h"

#include "engine/helper/math.h"

namespace tdrp::script::modules
{

void bind_vector(sol::state& lua)
{
	lua.new_usertype<Vector2df>("Vector2df", sol::constructors<Vector2df(), Vector2df(const Vector2df), Vector2df(float, float)>(),
		sol::meta_function::addition, sol::overload(
			sol::resolve<Vector2df(const Vector2df&, const Vector2df&)>(mathfu::operator+),
			sol::resolve<Vector2df(float, const Vector2df&)>(mathfu::operator+),
			sol::resolve<Vector2df(const Vector2df&, float)>(mathfu::operator+),
			sol::resolve<Vector2df(const Vector2df&, const Vector2di&)>(operator+)
		),

		sol::meta_function::subtraction, sol::overload(
			sol::resolve<Vector2df(const Vector2df&, const Vector2df&)>(mathfu::operator-),
			sol::resolve<Vector2df(float, const Vector2df&)>(mathfu::operator-),
			sol::resolve<Vector2df(const Vector2df&, float)>(mathfu::operator-),
			sol::resolve<Vector2df(const Vector2df&, const Vector2di&)>(operator-)
		),

		sol::meta_function::multiplication, sol::overload(
			sol::resolve<Vector2df(const Vector2df&, const Vector2df&)>(mathfu::operator*),
			sol::resolve<Vector2df(float, const Vector2df&)>(mathfu::operator*),
			sol::resolve<Vector2df(const Vector2df&, float)>(mathfu::operator*)
		),

		sol::meta_function::division, sol::overload(
			sol::resolve<Vector2df(const Vector2df&, const Vector2df&)>(mathfu::operator/),
			sol::resolve<Vector2df(const Vector2df&, float)>(mathfu::operator/)
		),

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

	lua.new_usertype<Vector2di>("Vector2di", sol::constructors<Vector2di(), Vector2di(const Vector2di), Vector2di(int, int)>(),
		sol::meta_function::addition, sol::overload(
			sol::resolve<Vector2di(const Vector2di&, const Vector2di&)>(mathfu::operator+),
			sol::resolve<Vector2di(int, const Vector2di&)>(mathfu::operator+),
			sol::resolve<Vector2di(const Vector2di&, int)>(mathfu::operator+),
			sol::resolve<Vector2di(const Vector2di&, const Vector2df&)>(operator+)
		),

		sol::meta_function::subtraction, sol::overload(
			sol::resolve<Vector2di(const Vector2di&, const Vector2di&)>(mathfu::operator-),
			sol::resolve<Vector2di(int, const Vector2di&)>(mathfu::operator-),
			sol::resolve<Vector2di(const Vector2di&, int)>(mathfu::operator-),
			sol::resolve<Vector2di(const Vector2di&, const Vector2df&)>(operator-)
		),

		sol::meta_function::multiplication, sol::overload(
			sol::resolve<Vector2di(const Vector2di&, const Vector2di&)>(mathfu::operator*),
			sol::resolve<Vector2di(int, const Vector2di&)>(mathfu::operator*),
			sol::resolve<Vector2di(const Vector2di&, int)>(mathfu::operator*)
		),

		sol::meta_function::division, sol::overload(
			sol::resolve<Vector2di(const Vector2di&, const Vector2di&)>(mathfu::operator/),
			sol::resolve<Vector2di(const Vector2di&, int)>(mathfu::operator/)
		),

		sol::meta_function::equal_to, sol::resolve<bool(const Vector2di&, const Vector2di&)>(mathfu::operator==),

		sol::meta_function::unary_minus, sol::resolve<Vector2di(const Vector2di&)>(mathfu::operator-),

		sol::meta_function::index, [](Vector2di& vector, int i) -> int { if (i == 0) return vector.x; else return vector.y; },
		sol::meta_function::new_index, [](Vector2di& vector, int i, int v) -> void { if (i == 0) vector.x = v; else vector.y = v; },

		"Length", &Vector2di::Length,
		"LengthSquared", &Vector2di::LengthSquared,
		"Normalize", &Vector2di::Normalize,
		"Normalized", &Vector2di::Normalized,

		"X", &Vector2di::x,
		"Y", &Vector2di::y,

		"Angle", &Vector2di::Angle,
		//"CrossProduct", &Vector2di::CrossProduct,
		"Distance", &Vector2di::Distance,
		"DistanceSquared", &Vector2di::DistanceSquared,
		"DotProduct", &Vector2di::DotProduct,
		"HadamardProduct", &Vector2di::HadamardProduct,
		"Lerp", &Vector2di::Lerp,
		"Max", &Vector2di::Max,
		"Min", &Vector2di::Min,
		"RandomInRange", &Vector2di::RandomInRange
	);

	lua.new_usertype<Rectf>("Rectf", sol::constructors<Rectf(), Rectf(const Rectf), Rectf(float, float, float, float), Rectf(Vector2df, Vector2df)>(),
		sol::meta_function::equal_to, sol::resolve<bool(const Rectf&, const Rectf&)>(mathfu::operator==),

		"Pos", &Rectf::pos,
		"Size", &Rectf::size
	);

	lua.new_usertype<Recti>("Recti", sol::constructors<Recti(), Recti(const Recti), Recti(int, int, int, int), Recti(Vector2di, Vector2di)>(),
		sol::meta_function::equal_to, sol::resolve<bool(const Recti&, const Recti&)>(mathfu::operator==),

		"Pos", &Recti::pos,
		"Size", &Recti::size
	);
}

} // end namespace tdrp::script::modules
