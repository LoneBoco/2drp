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
	ANGLE,
	SCALE_X,
	SCALE_Y,
	VELOCITY_X,
	VELOCITY_Y,
	VELOCITY_ANGLE,
	ACCELERATION_X,
	ACCELERATION_Y,
	ACCELERATION_ANGLE,
	DIRECTION,
	IMAGE,
	ENTITY,
	ANIMATION,
	TEXT,

	COUNT
};
// Any changes also go into ObjectProperties::create() and bind_attributes.


template <typename T> requires std::integral<T>
constexpr Property PropertyById(T id)
{
	if (id >= static_cast<T>(Property::COUNT))
		return Property::INVALID;
	return static_cast<Property>(id);
}

constexpr uint16_t PropertyToId(Property property)
{
	return static_cast<uint16_t>(property);
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

	//! Returns if we have any dirty attributes.
	bool HasDirty() const
	{
		return std::any_of(m_properties.begin(), m_properties.end(), [](const auto& v) -> bool
		{
			return v.second->Dirty;
		});
	}
	
	//! Processes all dirty attributes and returns if any were dirty.
	bool ProcessAllDirty() const
	{
		bool result = false;
		for (auto& [id, attr] : m_properties)
			result = attr->ProcessDirty() || result;
		return result;
	}

	//! Clears all the dirty flags and doesn't dispatch any events.
	void ClearAllDirty();

	EventDispatcher<uint16_t> DirtyUpdateDispatch;
	std::unordered_set<AttributeID> NetworkQueuedAttributes;

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
