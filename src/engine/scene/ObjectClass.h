#pragma once

#include "engine/common.h"

#include "ObjectAttributes.h"

namespace tdrp
{

class ObjectClass
{
public:
	//! Constructor.
	ObjectClass(const std::string& name) : m_name(name)
	{
	}

	//! Destructor.
	~ObjectClass()
	{
	}

	//! Gets the name of the object class.
	//! \return The name of the object class.
	std::string GetName() const		{ return m_name; }

	//! Object properties.
	ObjectAttributes Attributes;

private:
	std::string m_name;
};

} // end namespace tdrp
