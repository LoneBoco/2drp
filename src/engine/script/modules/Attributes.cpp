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

		"Set", sol::resolve<Attribute& (const std::string&)>(&Attribute::Set),
		"SetString", sol::resolve<Attribute& (const std::string&)>(&Attribute::Set),
		"SetSigned", sol::resolve<Attribute& (const int64_t)>(&Attribute::Set),
		"SetUnsigned", sol::resolve<Attribute& (const uint64_t)>(&Attribute::Set),
		"SetFloat", sol::resolve<Attribute& (const float)>(&Attribute::Set),
		"SetDouble", sol::resolve<Attribute& (const double)>(&Attribute::Set),

		sol::meta_function::new_index, sol::overload(
			sol::resolve<Attribute& (const std::string&)>(&Attribute::Set),
			sol::resolve<Attribute& (const int64_t)>(&Attribute::Set),
			sol::resolve<Attribute& (const uint64_t)>(&Attribute::Set),
			sol::resolve<Attribute& (const float)>(&Attribute::Set),
			sol::resolve<Attribute& (const double)>(&Attribute::Set)
		),

		"Subscribe", &subscribe,
		"Unsubscribe", [](Attribute& self, EventHandle ev) { self.UpdateDispatch.Unsubscribe(ev); }
	);

	lua.new_usertype<ObjectAttributes>("ObjectAttributes", sol::no_constructor,
		"AddAttribute", sol::overload(
			sol::resolve<std::weak_ptr<Attribute>(const std::string&, int64_t, uint16_t)>(&ObjectAttributes::AddAttribute),
			sol::resolve<std::weak_ptr<Attribute>(const std::string&, double, uint16_t)>(&ObjectAttributes::AddAttribute),
			sol::resolve<std::weak_ptr<Attribute>(const std::string&, const std::string&, uint16_t)>(&ObjectAttributes::AddAttribute)
		),

		sol::meta_function::index, sol::overload(
			[](ObjectAttributes& attributes, uint16_t id) -> std::shared_ptr<Attribute> { return attributes.Get(id); },
			[](ObjectAttributes& attributes, const std::string& name) -> std::shared_ptr<Attribute> { return attributes.Get(name); }
		),

		sol::meta_function::new_index, sol::overload(
			[](ObjectAttributes& attributes, uint16_t id, int64_t value) { auto a = attributes.Get(id); if (a == nullptr) throw "Invalid attribute ID"; a->Set(value); },
			[](ObjectAttributes& attributes, uint16_t id, double value) { auto a = attributes.Get(id); if (a == nullptr) throw "Invalid attribute ID"; a->Set(value); },
			[](ObjectAttributes& attributes, uint16_t id, const std::string& value) { auto a = attributes.Get(id); if (a == nullptr) throw "Invalid attribute ID"; a->Set(value); },
			[](ObjectAttributes& attributes, const std::string& name, int64_t value) { attributes.GetOrCreate(name)->Set(value); },
			[](ObjectAttributes& attributes, const std::string& name, double value) { attributes.GetOrCreate(name)->Set(value); },
			[](ObjectAttributes& attributes, const std::string& name, const std::string& value) { attributes.GetOrCreate(name)->Set(value); }
		)
	);

	lua.new_usertype<ObjectProperties>("ObjectProperties", sol::no_constructor,
		sol::meta_function::index, [](ObjectProperties& properties, Property prop) -> std::shared_ptr<Attribute> { return properties.Get(prop); },

		sol::meta_function::new_index, sol::overload(
			[](ObjectProperties& properties, Property prop, int64_t value) { auto p = properties.Get(prop); if (p == nullptr) throw "Invalid property"; p->Set(value); },
			[](ObjectProperties& properties, Property prop, double value) { auto p = properties.Get(prop); if (p == nullptr) throw "Invalid property"; p->Set(value); },
			[](ObjectProperties& properties, Property prop, const std::string& value) { auto p = properties.Get(prop); if (p == nullptr) throw "Invalid property"; p->Set(value); },
			[](ObjectProperties& properties, const std::string& name, int64_t value) { auto p = properties.Get(name); if (p == nullptr) throw "Invalid property"; p->Set(value); },
			[](ObjectProperties& properties, const std::string& name, double value) { auto p = properties.Get(name); if (p == nullptr) throw "Invalid property"; p->Set(value); },
			[](ObjectProperties& properties, const std::string& name, const std::string& value) { auto p = properties.Get(name); if (p == nullptr) throw "Invalid property"; p->Set(value); }
		)
	);
}

} // end namespace tdrp::script::modules
