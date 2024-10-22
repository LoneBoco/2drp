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
#define _PROP(x, y, z) m_properties.AddAttribute(#x + 10, y, (AttributeID)x); Get(x)->NetworkUpdateRate = z;

	_PROP(Property::LEVEL, "", 0ms);
	_PROP(Property::X, 0.0f, 15ms);
	_PROP(Property::Y, 0.0f, 15ms);
	_PROP(Property::Z, 0.0f, 15ms);
	_PROP(Property::ANGLE, 0.0f, 15ms);
	_PROP(Property::SCALE_X, 1.0f, 0ms);
	_PROP(Property::SCALE_Y, 1.0f, 0ms);
	_PROP(Property::VELOCITY_X, 0.0f, 0ms);
	_PROP(Property::VELOCITY_Y, 0.0f, 0ms);
	_PROP(Property::VELOCITY_ANGLE, 0.0f, 0ms);
	_PROP(Property::ACCELERATION_X, 0.0f, 0ms);
	_PROP(Property::ACCELERATION_Y, 0.0f, 0ms);
	_PROP(Property::ACCELERATION_ANGLE, 0.0f, 0ms);
	_PROP(Property::DIRECTION, (int64_t)0, 0ms);
	_PROP(Property::IMAGE, "", 0ms);
	_PROP(Property::ENTITY, "", 0ms);
	_PROP(Property::ANIMATION, "", 0ms);
	_PROP(Property::TEXT, "", 0ms);

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
		attribute->Dirty = false;
	}
}

} // end namespace tdrp
