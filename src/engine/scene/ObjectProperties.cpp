#include <sstream>

#include "engine/common.h"

#include "SceneObject.h"
#include "ObjectAttributes.h"
#include "ObjectProperties.h"

namespace tdrp
{

ObjectProperties::ObjectProperties()
{
	create();
}

ObjectProperties::ObjectProperties(const ObjectProperties& other)
{
	create();
	clone(other);
}

ObjectProperties::~ObjectProperties()
{
}

ObjectProperties::ObjectProperties(ObjectProperties&& other) noexcept
{
	m_properties = std::move(other.m_properties);
}

ObjectProperties& ObjectProperties::operator=(ObjectProperties&& other) noexcept
{
	m_properties = std::move(other.m_properties);
	return *this;
}

ObjectProperties& ObjectProperties::operator=(const ObjectProperties& other)
{
	clone(other);
	return *this;
}

const Attribute& ObjectProperties::operator[](const Property prop) const
{
	return *Get(prop);
}

Attribute& ObjectProperties::operator[](const Property prop)
{
	return *Get(prop);
}

std::shared_ptr<const Attribute> ObjectProperties::Get(const Property prop) const
{
	return m_properties.Get((uint16_t)prop);
}

std::shared_ptr<const Attribute> ObjectProperties::Get(const std::string& prop) const
{
	return m_properties.Get(prop);
}

std::shared_ptr<Attribute> ObjectProperties::Get(const Property prop)
{
	return m_properties.Get((uint16_t)prop);
}

std::shared_ptr<Attribute> ObjectProperties::Get(const std::string& prop)
{
	return m_properties.Get(prop);
}

void ObjectProperties::create()
{
// Add 10 to move the pointer past 'Property::' because I am horrible.
#define _PROP(x, y) m_properties.AddAttribute(#x + 10, y, (AttributeID)x);

	_PROP(Property::LEVEL, "");
	_PROP(Property::X, 0.0f);
	_PROP(Property::Y, 0.0f);
	_PROP(Property::Z, 0.0f);
	_PROP(Property::ANGLE, 0.0f);
	_PROP(Property::SCALE_X, 1.0f);
	_PROP(Property::SCALE_Y, 1.0f);
	_PROP(Property::VELOCITY_X, 0.0f);
	_PROP(Property::VELOCITY_Y, 0.0f);
	_PROP(Property::VELOCITY_ANGLE, 0.0f);
	_PROP(Property::ACCELERATION_X, 0.0f);
	_PROP(Property::ACCELERATION_Y, 0.0f);
	_PROP(Property::ACCELERATION_ANGLE, 0.0f);
	_PROP(Property::DIRECTION, (int64_t)0);
	_PROP(Property::IMAGE, "");
	_PROP(Property::ENTITY, "");
	_PROP(Property::ANIMATION, "");
	_PROP(Property::TEXT, "");

#undef _PROP
}

void ObjectProperties::clone(const ObjectProperties& properties)
{
	m_properties = properties.m_properties;
}

void ObjectProperties::ClearAllDirty()
{
	for (auto& [id, attribute] : m_properties)
	{
		attribute->ResetAllDirty();
	}
}

} // end namespace tdrp
