#include "engine/common.h"

#include "ObjectAttributes.h"


namespace tdrp
{

const AttributeType GetAttributeType(const AttributeVariant& value)
{
	if (std::holds_alternative<int64_t>(value))
		return AttributeType::INTEGER;
	if (std::holds_alternative<double>(value))
		return AttributeType::DOUBLE;
	if (std::holds_alternative<std::string>(value))
		return AttributeType::STRING;

	return AttributeType::INVALID;
}

///////////////////////////////////////////////////////////

Attribute::Attribute(Attribute&& o) noexcept
	: ID{ o.ID }, Name{ std::move(o.Name) }, Replicated{ o.Replicated }, Dirty{ o.Dirty }, NetworkUpdateRate{ o.NetworkUpdateRate }, m_value{ std::move(o.m_value) }, m_update_cooldown{ o.m_update_cooldown }
{}

Attribute& Attribute::operator=(const Attribute& other)
{
	ID = other.ID;
	Name = other.Name;
	Replicated = other.Replicated;
	Dirty = other.Dirty;
	NetworkUpdateRate = other.NetworkUpdateRate;
	Set(other.m_value);
	return *this;
}

Attribute& Attribute::operator=(Attribute&& other) noexcept
{
	ID = other.ID;
	Name = std::move(other.Name);
	Replicated = other.Replicated;
	Dirty = other.Dirty;
	NetworkUpdateRate = other.NetworkUpdateRate;
	m_value = std::move(other.m_value);

	other.ID = 0;
	return *this;
}

Attribute& Attribute::Set(const Attribute& other)
{
	return Set(other.m_value);
}

Attribute& Attribute::Set(const AttributeVariant& value)
{
	const auto me_t = GetType();
	const auto them_t = GetAttributeType(value);
	if (me_t != them_t)
		Dirty = true;
	else
	{
		// TODO: See what happens if the type changes.
		switch (me_t)
		{
			case AttributeType::INTEGER:
			{
				auto& me = std::get<int64_t>(m_value);
				auto& them = std::get<int64_t>(value);
				if (me != them) Dirty = true;
				break;
			}
			case AttributeType::DOUBLE:
			{
				auto& me = std::get<double>(m_value);
				auto& them = std::get<double>(value);
				if (me != them) Dirty = true;
				break;
			}
			case AttributeType::STRING:
			{
				auto& me = std::get<std::string>(m_value);
				auto& them = std::get<std::string>(value);
				if (me != them) Dirty = true;
				break;
			}
		}
	}

	m_value = value;
	return *this;
}

Attribute& Attribute::Set(const int64_t value)
{
	return Set(AttributeVariant{ value });
}

Attribute& Attribute::Set(const double value)
{
	return Set(AttributeVariant{ value });
}

Attribute& Attribute::Set(std::nullptr_t)
{
	if (GetType() != AttributeType::INVALID)
		Dirty = true;

	m_value = AttributeVariant{};

	return *this;
}

Attribute& Attribute::SetAsType(AttributeType type, const std::string& value)
{
	if (type == AttributeType::STRING)
	{
		Set(value);
		return *this;
	}

	std::istringstream str(value);
	int64_t r{};
	double d{};

	switch (type)
	{
	case AttributeType::INTEGER:
		str >> r;
		Set(AttributeVariant{ r });
		break;
	case AttributeType::DOUBLE:
		str >> d;
		Set(AttributeVariant{ d });
		break;
	}

	return *this;
}

Attribute& Attribute::operator=(const AttributeVariant& value)
{
	return Set(value);
}

Attribute& Attribute::operator=(const int64_t value)
{
	return Set(value);
}

Attribute& Attribute::operator=(const double value)
{
	return Set(value);
}

const AttributeType Attribute::GetType() const
{
	return GetAttributeType(m_value);
}

AttributeType Attribute::TypeFromString(const std::string& type)
{
	if (boost::iequals(type, "integer") || boost::iequals(type, "int"))
		return AttributeType::INTEGER;
	if (boost::iequals(type, "double") || boost::iequals(type, "float") || boost::iequals(type, "number") || boost::iequals(type, "real"))
		return AttributeType::DOUBLE;
	if (boost::iequals(type, "string"))
		return AttributeType::STRING;

	return AttributeType::INVALID;
}

std::string_view Attribute::TypeAsString() const
{
	const auto type = GetType();
	if (type == AttributeType::INTEGER)
		return "integer";
	if (type == AttributeType::DOUBLE)
		return "double";
	if (type == AttributeType::STRING)
		return "string";

	return "invalid";
}

///////////////////////////////////////////////////////////////////////////////

ObjectAttributes::ObjectAttributes(const ObjectAttributes& other)
: m_cid(0)
{
	for (auto& [id, a] : other.m_attributes)
		AddAttribute(a, id);
}

ObjectAttributes::~ObjectAttributes()
{
	m_attributes.clear();
}

ObjectAttributes::ObjectAttributes(ObjectAttributes&& other) noexcept
{
	m_cid = other.m_cid;
	m_attributes = std::move(other.m_attributes);

	other.m_cid = 0;
	other.m_attributes.clear();
}

ObjectAttributes& ObjectAttributes::operator=(ObjectAttributes&& other) noexcept
{
	m_cid = other.m_cid;
	m_attributes = std::move(other.m_attributes);

	other.m_cid = 0;
	other.m_attributes.clear();
	return *this;
}

ObjectAttributes& ObjectAttributes::operator=(const ObjectAttributes& other)
{
	m_attributes.clear();

	for (auto& [id, a] : other.m_attributes)
	{
		AddAttribute(a, id);
	}

	return *this;
}

std::shared_ptr<Attribute> ObjectAttributes::AddAttribute(const AttributePtr& a, AttributeID id)
{
	auto attr = getOrCreateAttribute(a->Name, id);
	attr->Set(*a);
	return attr;
}

std::shared_ptr<Attribute> ObjectAttributes::AddAttribute(const std::string& name, AttributeID id)
{
	return getOrCreateAttribute(name, id);
}

std::shared_ptr<Attribute> ObjectAttributes::AddAttribute(const std::string& name, AttributeType type, const std::string& value)
{
	auto attr = getOrCreateAttribute(name);
	attr->SetAsType(type, value);
	return attr;
}

std::shared_ptr<Attribute> ObjectAttributes::Get(const std::string& name)
{
	if (name.size() == 0) return nullptr;
	for (auto&& a : m_attributes)
	{
		if (boost::iequals(a.second->Name, name))
			return a.second;
	}
	return nullptr;
}

std::shared_ptr<Attribute> ObjectAttributes::Get(const AttributeID id)
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
		if (boost::iequals(a.second->Name, name))
			return a.second;
	}
	return nullptr;
}

std::shared_ptr<const Attribute> ObjectAttributes::Get(const AttributeID id) const
{
	auto i = m_attributes.find(id);
	if (i == m_attributes.end()) return nullptr;
	return i->second;
}

std::shared_ptr<Attribute> ObjectAttributes::GetOrCreate(const std::string& name)
{
	return getOrCreateAttribute(name, INVALID_ATTRIBUTE);
}

////////////////////////////

void ObjectAttributes::ClearAllDirty()
{
	for (auto& [id, attribute] : m_attributes)
	{
		attribute->Dirty = false;
	}
}

////////////////////////////

void ObjectAttributes::assignId(const AttributeID id, Attribute& prop)
{
	if (id != INVALID_ATTRIBUTE)
		prop.ID = id;
	else
	{
		prop.ID = m_cid;
		++m_cid;
	}
}

std::shared_ptr<Attribute> ObjectAttributes::getOrCreateAttribute(const std::string& name, AttributeID id)
{
	// Find the attribute.  Determine if we need to delete it.
	std::shared_ptr<Attribute> a = Get(name);
	if (a != nullptr) return a;

	a = std::make_shared<Attribute>();
	a->Name = name;
	assignId(id, *a);

	m_attributes.insert(std::pair<AttributeID, std::shared_ptr<Attribute>>(a->ID, a));
	return a;
}

} // end namespace tdrp
