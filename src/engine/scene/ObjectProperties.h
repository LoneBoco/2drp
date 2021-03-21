#pragma once

#include "engine/common.h"

#include "ObjectAttributes.h"

namespace tdrp
{

enum class Property
{
	INVALID = 0,
	LEVEL,
	X,
	Y,
	Z,
	SCALE_X,
	SCALE_Y,
	VELOCITY_X,
	VELOCITY_Y,
	FORCE_X,
	FORCE_Y,
	TORQUE,
	ROTATION,
	DIRECTION,
	IMAGE,

	COUNT
};

template <typename T> requires std::integral<T>
Property PropertyById(T id)
{
	if (id >= static_cast<T>(Property::COUNT))
		return Property::INVALID;
	return static_cast<Property>(id);
}


class SceneObject;
class ObjectProperties
{
public:
	ObjectProperties();
	ObjectProperties(const ObjectProperties& other);
	~ObjectProperties();

	ObjectProperties(ObjectProperties&& other) noexcept;
	ObjectProperties& operator=(ObjectProperties&& other) noexcept;

	ObjectProperties& operator=(const ObjectProperties& other);
	const Attribute& operator[](const Property prop) const;
	Attribute& operator[](const Property prop);

	//! Gets the specified property.
	//! \param prop The property to get.
	//! \return A pointer to the attribute.
	std::shared_ptr<const Attribute> Get(const Property prop) const;
	std::shared_ptr<const Attribute> Get(const std::string& prop) const;

	//! Gets the specified property.
	//! \param prop The property to get.
	//! \return A pointer to the attribute.
	std::shared_ptr<Attribute> Get(const Property prop);
	std::shared_ptr<Attribute> Get(const std::string& prop);

public:
	ObjectAttributes::attribute_map::iterator begin() { return m_properties.begin(); }
	ObjectAttributes::attribute_map::iterator end() { return m_properties.end(); }
	ObjectAttributes::attribute_map::const_iterator begin() const { return m_properties.begin(); }
	ObjectAttributes::attribute_map::const_iterator end() const { return m_properties.end(); }

private:
	void create();
	void clone(const ObjectProperties& properties);
	ObjectAttributes m_properties;
};

} // end namespace tdrp
