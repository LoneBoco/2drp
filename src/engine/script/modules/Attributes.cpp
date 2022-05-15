#include "engine/script/modules/bind.h"

#include "engine/scene/ObjectAttributes.h"
#include "engine/scene/ObjectProperties.h"

namespace sol
{
	template <>
	struct is_container<tdrp::ObjectAttributes> : std::false_type {};

	template <>
	struct is_container<tdrp::ObjectProperties> : std::false_type {};
}

namespace tdrp::script::modules
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


void bind_attributes(sol::state& lua)
{
	lua.new_enum("AttributeType",
		"INVALID", AttributeType::INVALID,
		"SIGNED", AttributeType::SIGNED,
		"UNSIGNED", AttributeType::UNSIGNED,
		"FLOAT", AttributeType::FLOAT,
		"DOUBLE", AttributeType::DOUBLE,
		"STRING", AttributeType::STRING
	);

	lua["NEW_ID"] = &INVALID_ATTRIBUTE;

	lua.new_enum("Property",
		"INVALID", Property::INVALID,
		"LEVEL", Property::LEVEL,
		"X", Property::X,
		"Y", Property::Y,
		"Z", Property::Z,
		"SCALE_X", Property::SCALE_X,
		"SCALE_Y", Property::SCALE_Y,
		"VELOCITY_X", Property::VELOCITY_X,
		"VELOCITY_Y", Property::VELOCITY_Y,
		"FORCE_X", Property::FORCE_X,
		"FORCE_Y", Property::FORCE_Y,
		"TORQUE", Property::TORQUE,
		"ROTATION", Property::ROTATION,
		"DIRECTION", Property::DIRECTION,
		"IMAGE", Property::IMAGE,
		"ENTITY", Property::ENTITY,
		"ANIMATION", Property::ANIMATION
	);

	using AttributeDispatcher = EventDispatcher<uint16_t>;

	lua.new_usertype<Attribute>("Attribute", sol::no_constructor,
		"ID", sol::readonly_property(&Attribute::GetId),
		"Type", sol::readonly_property(&Attribute::GetType),
		"AsString", &Attribute::GetString,
		"AsSigned", &Attribute::GetSigned,
		"AsUnsigned", &Attribute::GetUnsigned,
		"AsFloat", &Attribute::GetFloat,
		"AsDouble", &Attribute::GetDouble,

		"Get", &asObject,
		"Set", &fromObject,

		"SetString", sol::resolve<Attribute& (const std::string&)>(&Attribute::Set),
		"SetSigned", sol::resolve<Attribute& (const int64_t)>(&Attribute::Set),
		"SetUnsigned", sol::resolve<Attribute& (const uint64_t)>(&Attribute::Set),
		"SetFloat", sol::resolve<Attribute& (const float)>(&Attribute::Set),
		"SetDouble", sol::resolve<Attribute& (const double)>(&Attribute::Set),

		/*
		sol::meta_function::index, [](Attribute& self, sol::this_state s) { return asObject(self, s); },

		sol::meta_function::new_index, sol::overload(
			sol::resolve<Attribute& (const std::string&)>(&Attribute::Set),
			sol::resolve<Attribute& (const int64_t)>(&Attribute::Set),
			sol::resolve<Attribute& (const uint64_t)>(&Attribute::Set),
			sol::resolve<Attribute& (const float)>(&Attribute::Set),
			sol::resolve<Attribute& (const double)>(&Attribute::Set)
		),
		*/

		"Subscribe", &subscribe,
		"Unsubscribe", [](Attribute& self, EventHandle ev) { self.UpdateDispatch.Unsubscribe(ev); }
	);

	lua.new_usertype<ObjectAttributes>("ObjectAttributes", sol::no_constructor,
		"Add", sol::overload(
			sol::resolve<std::weak_ptr<Attribute>(const std::string&, int64_t, uint16_t)>(&ObjectAttributes::AddAttribute),
			sol::resolve<std::weak_ptr<Attribute>(const std::string&, double, uint16_t)>(&ObjectAttributes::AddAttribute),
			sol::resolve<std::weak_ptr<Attribute>(const std::string&, const std::string&, uint16_t)>(&ObjectAttributes::AddAttribute)
		),

		"Get", sol::overload(
			sol::resolve<AttributePtr(const std::string&)>(&ObjectAttributes::Get),
			sol::resolve<AttributePtr(const uint16_t)>(&ObjectAttributes::Get)
		),

		sol::meta_function::index, sol::overload(
			[](ObjectAttributes& attributes, const std::string& name, sol::this_state s) { return asObject(attributes.Get(name), s); },
			[](ObjectAttributes& attributes, uint16_t id, sol::this_state s) { return asObject(attributes.Get(id), s); }
		),

		sol::meta_function::new_index, sol::overload(
			[](ObjectAttributes& attributes, const std::string& name, const sol::object& value)
			{
				auto a = attributes.GetOrCreate(name);
				fromObject(*a, value);
			},
			[](ObjectAttributes& attributes, uint16_t id, const sol::object& value)
			{
				auto a = attributes.Get(id);
				if (a == nullptr) throw "Invalid attribute ID";
				fromObject(*a, value);
			}
		)
	);

	lua.new_usertype<ObjectProperties>("ObjectProperties", sol::no_constructor,
		"Get", sol::overload(
			sol::resolve<AttributePtr(const std::string&)>(&ObjectProperties::Get),
			sol::resolve<AttributePtr (const Property)>(&ObjectProperties::Get)
		),

		sol::meta_function::index, sol::overload(
			[](ObjectProperties& properties, Property prop, sol::this_state s) { return asObject(properties.Get(prop), s); },
			[](ObjectProperties& properties, const std::string& name, sol::this_state s) { return asObject(properties.Get(name), s); }
		),

		sol::meta_function::new_index, sol::overload(
			[](ObjectProperties& properties, Property prop, const sol::object& value)
			{
				auto p = properties.Get(prop);
				if (p == nullptr) throw "Invalid property";
				fromObject(*p, value);
			},
			[](ObjectProperties& properties, const std::string& name, const sol::object& value)
			{
				auto p = properties.Get(name);
				if (p == nullptr) throw "Invalid property";
				fromObject(*p, value);
			}
		)
	);
}

} // end namespace tdrp::script::modules
