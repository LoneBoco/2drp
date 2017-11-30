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

class SceneObject;
class ObjectProperties
{
public:
	ObjectProperties();
	ObjectProperties(const ObjectProperties& properties);
	~ObjectProperties();

	void operator=(const ObjectProperties& properties);

	//! Gets the specified property.
	//! \param prop The property to get.
	//! \return A pointer to the attribute.
	std::shared_ptr<Attribute> Get(const Property prop);

	//! Gets the specified property.
	//! \param prop The property to get.
	//! \return A pointer to the attribute.
	std::shared_ptr<const Attribute> Get(const Property prop) const;

private:
	void create();
	void clone(const ObjectProperties& properties);
	ObjectAttributes m_properties;
};

} // end namespace tdrp
