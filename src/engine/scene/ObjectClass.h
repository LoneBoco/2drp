#pragma once

#include "engine/common.h"

#include "ObjectAttributes.h"

namespace tdrp
{

class ObjectClass
{
public:
	ObjectClass() = delete;
	ObjectClass(const std::string& name) : m_name(name) {}
	~ObjectClass() = default;

	ObjectClass(const ObjectClass& other) = delete;
	ObjectClass(ObjectClass&& other) = delete;
	ObjectClass& operator=(const ObjectClass& other) = delete;
	ObjectClass& operator=(ObjectClass&& other) = delete;

	//! Gets the name of the object class.
	//! \return The name of the object class.
	std::string GetName() const		{ return m_name; }

	//! Object properties.
	ObjectAttributes Attributes;

private:
	std::string m_name;
};

} // end namespace tdrp
