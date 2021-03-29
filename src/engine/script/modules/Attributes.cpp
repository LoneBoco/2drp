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

	lua.new_usertype<Attribute>("Attribute", sol::no_constructor,
		"ID", sol::readonly_property(&Attribute::GetId),
		"Type", sol::readonly_property(&Attribute::GetType)
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
			[](ObjectAttributes& attributes, uint16_t id, int64_t value) { attributes.Get(id)->Set(value); },
			[](ObjectAttributes& attributes, uint16_t id, double value) { attributes.Get(id)->Set(value); },
			[](ObjectAttributes& attributes, uint16_t id, const std::string& value) { attributes.Get(id)->Set(value); },
			[](ObjectAttributes& attributes, const std::string& name, int64_t value) { attributes.Get(name)->Set(value); },
			[](ObjectAttributes& attributes, const std::string& name, double value) { attributes.Get(name)->Set(value); },
			[](ObjectAttributes& attributes, const std::string& name, const std::string& value) { attributes.Get(name)->Set(value); }
		)
	);

	lua.new_usertype<ObjectProperties>("ObjectProperties", sol::no_constructor,
		sol::meta_function::index, [](ObjectProperties& properties, Property prop) -> std::shared_ptr<Attribute> { return properties.Get(prop); },

		sol::meta_function::new_index, sol::overload(
			[](ObjectProperties& properties, Property prop, int64_t value) { properties.Get(prop)->Set(value); },
			[](ObjectProperties& properties, Property prop, double value) { properties.Get(prop)->Set(value); },
			[](ObjectProperties& properties, Property prop, const std::string& value) { properties.Get(prop)->Set(value); },
			[](ObjectProperties& properties, const std::string& name, int64_t value) { properties.Get(name)->Set(value); },
			[](ObjectProperties& properties, const std::string& name, double value) { properties.Get(name)->Set(value); },
			[](ObjectProperties& properties, const std::string& name, const std::string& value) { properties.Get(name)->Set(value); }
		)
	);
}

} // end namespace tdrp::script::modules
