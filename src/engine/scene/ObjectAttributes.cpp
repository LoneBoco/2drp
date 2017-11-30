#include <map>

#include "engine/common.h"

#include "ObjectAttributes.h"

namespace tdrp
{

ObjectAttributes::ObjectAttributes(ObjectAttributes& props)
: m_cid(0)
{
	for (auto i = props.m_attributes.begin(); i != props.m_attributes.end(); ++i)
	{
		uint16_t id = i->first;
		Attribute* a = i->second;

		switch (a->GetType())
		{
			case AttributeType::SIGNED:
				AddAttribute(a->GetName(), a->GetSigned(), id);
				break;

			case AttributeType::UNSIGNED:
				AddAttribute(a->GetName(), a->GetUnsigned(), id);
				break;

			case AttributeType::FLOAT:
				AddAttribute(a->GetName(), a->GetFloat(), id);
				break;

			case AttributeType::DOUBLE:
				AddAttribute(a->GetName(), a->GetDouble(), id);
				break;

			case AttributeType::STRING:
				AddAttribute(a->GetName(), a->GetString(), id);
				break;
		}
	}
}

ObjectAttributes::~ObjectAttributes()
{
	for (auto i = m_attributes.begin(); i != m_attributes.end(); ++i)
		delete i->second;

	m_attributes.clear();
}


void ObjectAttributes::operator=(ObjectAttributes& props)
{
	for (auto i = m_attributes.begin(); i != m_attributes.end(); ++i)
		delete i->second;

	m_attributes.clear();

	for (auto i = props.m_attributes.begin(); i != props.m_attributes.end(); ++i)
	{
		uint16_t id = i->first;
		Attribute* a = i->second;

		switch (a->GetType())
		{
			case AttributeType::SIGNED:
				AddAttribute(a->GetName(), a->GetSigned(), id);
				break;

			case AttributeType::UNSIGNED:
				AddAttribute(a->GetName(), a->GetUnsigned(), id);
				break;

			case AttributeType::FLOAT:
				AddAttribute(a->GetName(), a->GetFloat(), id);
				break;

			case AttributeType::DOUBLE:
				AddAttribute(a->GetName(), a->GetDouble(), id);
				break;

			case AttributeType::STRING:
				AddAttribute(a->GetName(), a->GetString(), id);
				break;
		}
	}
}

Attribute* ObjectAttributes::AddAttribute(const std::string& name, int32_t id)
{
	// Get or create our new attribute.
	return getOrCreateAttribute(name, id);
}

Attribute* ObjectAttributes::AddAttribute(const std::string& name, int64_t value, int32_t id)
{
	// Get or create our new attribute.
	Attribute* a = getOrCreateAttribute(name, id);

	// Assign the new attribute.
	a->Set(value);
	return a;
}

Attribute* ObjectAttributes::AddAttribute(const std::string& name, uint64_t value, int32_t id)
{
	// Get or create our new attribute.
	Attribute* a = getOrCreateAttribute(name, id);

	// Assign the new attribute.
	a->Set(value);
	return a;
}

Attribute* ObjectAttributes::AddAttribute(const std::string& name, float value, int32_t id)
{
	// Get or create our new attribute.
	Attribute* a = getOrCreateAttribute(name, id);

	// Assign the new attribute.
	a->Set(value);
	return a;
}

Attribute* ObjectAttributes::AddAttribute(const std::string& name, double value, int32_t id)
{
	// Get or create our new attribute.
	Attribute* a = getOrCreateAttribute(name, id);

	// Assign the new attribute.
	a->Set(value);
	return a;
}

Attribute* ObjectAttributes::AddAttribute(const std::string& name, const std::string& value, int32_t id)
{
	// Get or create our new attribute.
	Attribute* a = getOrCreateAttribute(name, id);

	// Assign the new attribute.
	a->Set(value);
	return a;
}

Attribute* ObjectAttributes::Get(const std::string& name) const
{
	if (name.size() == 0) return 0;
	for (auto i = m_attributes.begin(); i != m_attributes.end(); ++i)
	{
		if (i->second->GetName() == name)
			return i->second;
	}
	return 0;
}

Attribute* ObjectAttributes::Get(const uint16_t id) const
{
	auto i = m_attributes.find(id);
	if (i == m_attributes.end()) return 0;
	return i->second;
}

////////////////////////////

void ObjectAttributes::assignId(const int32_t id, Attribute* prop)
{
	if (id == -1)
	{
		prop->SetId((uint16_t)m_cid);
		++m_cid;
	}
	else
		prop->SetId((uint16_t)id);
}

Attribute* ObjectAttributes::getOrCreateAttribute(const std::string& name, int32_t id)
{
	// Find the attribute.  Determine if we need to delete it.
	Attribute* a = Get(name);
	if (a != nullptr) return a;

	a = new Attribute(AttributeType::UNSIGNED);
	a->SetName(name);
	assignId(id, a);

	m_attributes.insert(std::pair<uint16_t, Attribute*>(a->GetId(), a));
	return a;
}

} // end namespace tdrp
