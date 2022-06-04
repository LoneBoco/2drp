#include "engine/script/helpers/AttributeHelper.h"

namespace tdrp::script::helpers
{

EventHandle subscribe(Attribute& self, sol::protected_function func)
{
	return self.UpdateDispatch.Subscribe(
		[&self, func](uint16_t id)
		{
			func.call(self);
		}
	);
}

sol::object asObject(const AttributePtr& attribute, sol::this_state s)
{
	sol::state_view lua{ s };
	if (attribute == nullptr)
		return sol::make_object(lua, sol::lua_nil);

	switch (attribute->GetType())
	{
	case AttributeType::SIGNED:
		return sol::object(lua, sol::in_place_type<int64_t>, attribute->GetSigned());
	case AttributeType::UNSIGNED:
		return sol::object(lua, sol::in_place_type<uint64_t>, attribute->GetUnsigned());
	case AttributeType::FLOAT:
		return sol::object(lua, sol::in_place_type<float>, attribute->GetFloat());
	case AttributeType::DOUBLE:
		return sol::object(lua, sol::in_place_type<double>, attribute->GetDouble());
	case AttributeType::STRING:
		return sol::make_object(lua, attribute->GetString());
	case AttributeType::INVALID:
	default:
		return sol::make_object(lua, sol::lua_nil);
	}
}

void fromObject(Attribute& attribute, const sol::object& object)
{
	if (object.is<int64_t>())
		attribute.Set(object.as<int64_t>());
	else if (object.is<uint64_t>())
		attribute.Set(object.as<uint64_t>());
	else if (object.is<float>())
		attribute.Set(object.as<float>());
	else if (object.is<double>())
		attribute.Set(object.as<double>());
	else if (object.is<std::string>())
		attribute.Set(object.as<std::string>());
	else if (object.is<Attribute>())
		attribute.Set(object.as<Attribute>());
	else attribute.Set(nullptr);
}

} // end namespace tdrp::script::helpers
