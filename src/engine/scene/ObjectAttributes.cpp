#include "engine/common.h"

#include "ObjectAttributes.h"

namespace tdrp
{

ObjectAttributes::ObjectAttributes(const ObjectAttributes& props)
: m_cid(0)
{
	for (auto i = props.m_attributes.begin(); i != props.m_attributes.end(); ++i)
	{
		uint16_t id = i->first;
		std::shared_ptr<Attribute> a = i->second;

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
	m_attributes.clear();
}


void ObjectAttributes::operator=(const ObjectAttributes& props)
{
	m_attributes.clear();

	for (auto i = props.m_attributes.begin(); i != props.m_attributes.end(); ++i)
	{
		uint16_t id = i->first;
		std::shared_ptr<Attribute> a = i->second;

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

std::weak_ptr<Attribute> ObjectAttributes::AddAttribute(const std::string& name, uint16_t id)
{
	// Get or create our new attribute.
	return getOrCreateAttribute(name, id);
}

std::weak_ptr<Attribute> ObjectAttributes::AddAttribute(const std::string& name, int64_t value, uint16_t id)
{
	std::shared_ptr<Attribute> a = getOrCreateAttribute(name, id);
	if (a) a->Set(value);
	return a;
}

std::weak_ptr<Attribute> ObjectAttributes::AddAttribute(const std::string& name, uint64_t value, uint16_t id)
{
	std::shared_ptr<Attribute> a = getOrCreateAttribute(name, id);
	if (a) a->Set(value);
	return a;
}

std::weak_ptr<Attribute> ObjectAttributes::AddAttribute(const std::string& name, float value, uint16_t id)
{
	std::shared_ptr<Attribute> a = getOrCreateAttribute(name, id);
	if (a) a->Set(value);
	return a;
}

std::weak_ptr<Attribute> ObjectAttributes::AddAttribute(const std::string& name, double value, uint16_t id)
{
	std::shared_ptr<Attribute> a = getOrCreateAttribute(name, id);
	if (a) a->Set(value);
	return a;
}

std::weak_ptr<Attribute> ObjectAttributes::AddAttribute(const std::string& name, const std::string& value, uint16_t id)
{
	std::shared_ptr<Attribute> a = getOrCreateAttribute(name, id);
	if (a) a->Set(value);
	return a;
}

std::shared_ptr<Attribute> ObjectAttributes::Get(const std::string& name)
{
	if (name.size() == 0) return nullptr;
	for (auto&& a : m_attributes)
	{
		if (a.second->GetName() == name)
			return a.second;
	}
	return nullptr;
}

std::shared_ptr<Attribute> ObjectAttributes::Get(const uint16_t id)
{
	auto i = m_attributes.find(id);
	if (i == m_attributes.end()) return nullptr;
	return i->second;
}

std::shared_ptr<const Attribute> ObjectAttributes::Get(const std::string& name) const
{
	if (name.size() == 0) return nullptr;
	for (auto&& a : m_attributes)
	{
		if (a.second->GetName() == name)
			return a.second;
	}
	return nullptr;
}

std::shared_ptr<const Attribute> ObjectAttributes::Get(const uint16_t id) const
{
	auto i = m_attributes.find(id);
	if (i == m_attributes.end()) return nullptr;
	return i->second;
}

////////////////////////////

void ObjectAttributes::assignId(const uint16_t id, Attribute& prop)
{
	if (id != -1)
		prop.SetId(id);
	else
	{
		prop.SetId(m_cid);
		++m_cid;
	}
}

std::shared_ptr<Attribute> ObjectAttributes::getOrCreateAttribute(const std::string& name, uint16_t id)
{
	// Find the attribute.  Determine if we need to delete it.
	std::shared_ptr<Attribute> a = Get(name);
	if (a != nullptr) return a;

	a = std::make_shared<Attribute>(AttributeType::UNSIGNED);
	a->SetName(name);
	assignId(id, *a);

	m_attributes.insert(std::pair<uint16_t, std::shared_ptr<Attribute>>(a->GetId(), a));
	return a;
}

} // end namespace tdrp
