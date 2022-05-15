#include "engine/common.h"

#include "ObjectAttributes.h"

namespace tdrp
{

Attribute::Attribute(Attribute&& o) noexcept
	: m_id(o.m_id), m_type(o.m_type), m_isDirty(false), m_value_int(o.m_value_int), m_value_float(o.m_value_float), m_value_double(o.m_value_double), m_value_string(std::move(o.m_value_string))
{
	o.m_id = 0;
	o.m_type = AttributeType::INVALID;
	o.m_isDirty = false;
	o.m_value_int.u = 0;
	o.m_value_float = 0.0f;
	o.m_value_double = 0.0;
}

Attribute& Attribute::operator=(const Attribute& other)
{
	m_id = other.m_id;
	m_type = other.m_type;
	m_isDirty = other.m_isDirty;
	m_value_int.u = other.m_value_int.u;
	m_value_float = other.m_value_float;
	m_value_double = other.m_value_double;
	m_value_string = other.m_value_string;
	return *this;
}

Attribute& Attribute::operator=(Attribute&& other) noexcept
{
	m_id = other.m_id;
	m_type = other.m_type;
	m_isDirty = other.m_isDirty;
	m_value_int.u = other.m_value_int.u;
	m_value_float = other.m_value_float;
	m_value_double = other.m_value_double;
	m_value_string = std::move(other.m_value_string);

	other.m_id = 0;
	other.m_type = AttributeType::INVALID;
	other.m_isDirty = false;
	other.m_value_int.u = 0;
	other.m_value_float = 0.0f;
	other.m_value_double = 0.0;
	return *this;
}

Attribute& Attribute::Set(const int64_t value)
{
	if (m_type != AttributeType::SIGNED || m_value_int.s != value)
		m_isDirty = true;

	m_value_string.clear();
	m_value_int.s = value;
	m_type = AttributeType::SIGNED;

	return *this;
}

Attribute& Attribute::Set(const uint64_t value)
{
	if (m_type != AttributeType::UNSIGNED || m_value_int.u != value)
		m_isDirty = true;

	m_value_string.clear();
	m_value_int.u = value;
	m_type = AttributeType::UNSIGNED;

	return *this;
}

Attribute& Attribute::Set(const float value)
{
	if (m_type != AttributeType::FLOAT || m_value_float != value)
		m_isDirty = true;

	m_value_string.clear();
	m_value_float = value;
	m_type = AttributeType::FLOAT;

	return *this;
}

Attribute& Attribute::Set(const double value)
{
	if (m_type != AttributeType::DOUBLE || m_value_double != value)
		m_isDirty = true;

	m_value_string.clear();
	m_value_double = value;
	m_type = AttributeType::DOUBLE;

	return *this;
}

Attribute& Attribute::Set(const std::string& value)
{
	if (m_type != AttributeType::STRING || m_value_string != value)
		m_isDirty = true;

	m_value_string = value;
	m_type = AttributeType::STRING;

	return *this;
}

Attribute& Attribute::Set(const Attribute& other)
{
	switch (other.m_type)
	{
	case AttributeType::SIGNED:
		return Set(other.m_value_int.s);
	case AttributeType::UNSIGNED:
		return Set(other.m_value_int.u);
	case AttributeType::FLOAT:
		return Set(other.m_value_float);
	case AttributeType::DOUBLE:
		return Set(other.m_value_double);
	case AttributeType::STRING:
		return Set(other.m_value_string);
	}

	return *this;
}

Attribute& Attribute::Set(std::nullptr_t)
{
	if (m_type != AttributeType::INVALID)
		m_isDirty = true;

	m_type = AttributeType::INVALID;

	return *this;
}

Attribute& Attribute::SetAsType(const AttributeType type, const std::string& value)
{
	if (type == AttributeType::STRING)
	{
		Set(value);
		return *this;
	}

	std::istringstream str(value);
	switch (type)
	{
	case AttributeType::SIGNED:
		int64_t s;
		str >> s;
		Set(s);
		break;
	case AttributeType::UNSIGNED:
		uint64_t u;
		str >> u;
		Set(u);
		break;
	case AttributeType::FLOAT:
		float f;
		str >> f;
		Set(f);
		break;
	case AttributeType::DOUBLE:
		double d;
		str >> d;
		Set(d);
		break;
	}
	return *this;
}

Attribute& Attribute::operator=(const int64_t value)
{
	return Set(value);
}
Attribute& Attribute::operator=(const uint64_t value)
{
	return Set(value);
}
Attribute& Attribute::operator=(const float value)
{
	return Set(value);
}
Attribute& Attribute::operator=(const double value)
{
	return Set(value);
}
Attribute& Attribute::operator=(const std::string& value)
{
	return Set(value);
}

int64_t Attribute::GetSigned() const
{
	switch (m_type)
	{
		case AttributeType::SIGNED:
			return m_value_int.s;
		case AttributeType::UNSIGNED:
			return static_cast<int64_t>(m_value_int.u);
		case AttributeType::FLOAT:
			return static_cast<int64_t>(m_value_float);
		case AttributeType::DOUBLE:
			return static_cast<int64_t>(m_value_double);
		case AttributeType::STRING:
		{
			std::istringstream str(m_value_string);
			int64_t r;
			str >> r;
			return r;
		}
	}
	return 0;
}

uint64_t Attribute::GetUnsigned() const
{
	switch (m_type)
	{
		case AttributeType::SIGNED:
			return static_cast<uint64_t>(m_value_int.s);
		case AttributeType::UNSIGNED:
			return m_value_int.u;
		case AttributeType::FLOAT:
			return static_cast<uint64_t>(m_value_float);
		case AttributeType::DOUBLE:
			return static_cast<uint64_t>(m_value_double);
		case AttributeType::STRING:
		{
			std::istringstream str(m_value_string);
			uint64_t r;
			str >> r;
			return r;
		}
	}
	return 0;
}

float Attribute::GetFloat() const
{
	switch (m_type)
	{
		case AttributeType::SIGNED:
			return static_cast<float>(m_value_int.s);
		case AttributeType::UNSIGNED:
			return static_cast<float>(m_value_int.u);
		case AttributeType::FLOAT:
			return m_value_float;
		case AttributeType::DOUBLE:
			return static_cast<float>(m_value_double);
		case AttributeType::STRING:
		{
			std::istringstream str(m_value_string);
			float r;
			str >> r;
			return r;
		}
	}
	return 0;
}

double Attribute::GetDouble() const
{
	switch (m_type)
	{
		case AttributeType::SIGNED:
			return static_cast<double>(m_value_int.s);
		case AttributeType::UNSIGNED:
			return static_cast<double>(m_value_int.u);
		case AttributeType::FLOAT:
			return static_cast<double>(m_value_float);
		case AttributeType::DOUBLE:
			return m_value_double;
		case AttributeType::STRING:
		{
			std::istringstream str(m_value_string);
			double r;
			str >> r;
			return r;
		}
	}
	return 0;
}

std::string Attribute::GetString() const
{
	std::stringstream str;
	switch (m_type)
	{
		case AttributeType::SIGNED:
			str << m_value_int.s;
			break;
		case AttributeType::UNSIGNED:
			str << m_value_int.u;
			break;
		case AttributeType::FLOAT:
			str << m_value_float;
			break;
		case AttributeType::DOUBLE:
			str << m_value_double;
			break;
		case AttributeType::STRING:
			return m_value_string;
	}
	return str.str();
}

AttributeType Attribute::TypeFromString(const std::string& type)
{
	if (boost::iequals(type, "signed"))
		return AttributeType::SIGNED;
	if (boost::iequals(type, "unsigned"))
		return AttributeType::UNSIGNED;
	if (boost::iequals(type, "float"))
		return AttributeType::FLOAT;
	if (boost::iequals(type, "double"))
		return AttributeType::DOUBLE;
	if (boost::iequals(type, "string"))
		return AttributeType::STRING;

	return AttributeType::INVALID;
}

///////////////////////////////////////////////////////////////////////////////

ObjectAttributes::ObjectAttributes(const ObjectAttributes& other)
: m_cid(0)
{
	for (auto i = other.m_attributes.begin(); i != other.m_attributes.end(); ++i)
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

	for (auto i = other.m_attributes.begin(); i != other.m_attributes.end(); ++i)
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

	return *this;
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

std::weak_ptr<Attribute> ObjectAttributes::AddAttribute(const std::string& name, const AttributeType type, const std::string& value, uint16_t id)
{
	std::shared_ptr<Attribute> a = getOrCreateAttribute(name, id);
	if (a) a->SetAsType(type, value);
	return a;
}

std::shared_ptr<Attribute> ObjectAttributes::Get(const std::string& name)
{
	if (name.size() == 0) return nullptr;
	for (auto&& a : m_attributes)
	{
		if (boost::iequals(a.second->GetName(), name))
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
		if (boost::iequals(a.second->GetName(), name))
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

std::shared_ptr<Attribute> ObjectAttributes::GetOrCreate(const std::string& name)
{
	return getOrCreateAttribute(name, INVALID_ATTRIBUTE);
}

////////////////////////////

void ObjectAttributes::ClearDirty()
{
	for (auto& [key, value] : m_attributes)
	{
		if (value->GetIsDirty())
		{
			value->SetIsDirty(false);
			value->UpdateDispatch.Post(key);
			DirtyUpdateDispatch.Post(key);
		}
	}
}

////////////////////////////

void ObjectAttributes::assignId(const uint16_t id, Attribute& prop)
{
	if (id != INVALID_ATTRIBUTE)
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
