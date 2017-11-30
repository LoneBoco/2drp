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
	ObjectProperties(ObjectProperties& properties);
	~ObjectProperties();

	void operator=(ObjectProperties& properties);

	//! Gets the specified property.
	Attribute* Get(Property prop) const;

	//! Gets the attributes directly.
	ObjectAttributes& GetAttributes()
	{
		return m_properties;
	}

private:
	void create();
	void clone(ObjectProperties& properties);
	ObjectAttributes m_properties;
};

} // end namespace tdrp
