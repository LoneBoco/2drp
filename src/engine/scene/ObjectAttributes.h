#pragma once

#include <sstream>

#include "engine/common.h"

namespace tdrp
{

enum class AttributeType
{
	INVALID		= 0,
	SIGNED,
	UNSIGNED,
	FLOAT,
	DOUBLE,
	STRING,

	COUNT
};

class Attribute
{
public:
	Attribute(const AttributeType t = AttributeType::INVALID) : m_id(0), m_type(t), m_isDirty(false), m_value_float(0.0f), m_value_double(0.0) { m_value_int.s = 0; }
	Attribute(const uint16_t i, const AttributeType t) : m_id(i), m_type(t), m_isDirty(false), m_value_float(0.0f), m_value_double(0.0) { m_value_int.s = 0; }
	Attribute(const Attribute& a) : m_id(a.m_id), m_type(a.m_type), m_isDirty(false), m_value_int(a.m_value_int), m_value_float(a.m_value_float), m_value_double(a.m_value_double), m_value_string(a.m_value_string) {}
	Attribute(Attribute&& o) : m_id(o.m_id), m_type(o.m_type), m_isDirty(false), m_value_int(o.m_value_int), m_value_float(o.m_value_float), m_value_double(o.m_value_double), m_value_string(std::move(o.m_value_string)) {}
	~Attribute() {}

	Attribute& Set(const int64_t value)
	{
		if (m_type != AttributeType::SIGNED || m_value_int.s != value)
			m_isDirty = true;

		m_value_string.clear();
		m_value_int.s = value;
		m_type = AttributeType::SIGNED;

		return *this;
	}

	Attribute& Set(const uint64_t value)
	{
		if (m_type != AttributeType::UNSIGNED || m_value_int.u != value)
			m_isDirty = true;

		m_value_string.clear();
		m_value_int.u = value;
		m_type = AttributeType::UNSIGNED;

		return *this;
	}

	Attribute& Set(const float value)
	{
		if (m_type != AttributeType::FLOAT || m_value_float != value)
			m_isDirty = true;

		m_value_string.clear();
		m_value_float = value;
		m_type = AttributeType::FLOAT;

		return *this;
	}

	Attribute& Set(const double value)
	{
		if (m_type != AttributeType::DOUBLE || m_value_double != value)
			m_isDirty = true;

		m_value_string.clear();
		m_value_double = value;
		m_type = AttributeType::DOUBLE;

		return *this;
	}

	Attribute& Set(const std::string& value)
	{
		if (m_type != AttributeType::STRING || m_value_string != value)
			m_isDirty = true;

		m_value_string = value;
		m_type = AttributeType::STRING;

		return *this;
	}

	Attribute& SetAsType(const AttributeType type, const std::string& value)
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

	Attribute& operator=(const int64_t value)
	{
		Set(value);
		return *this;
	}
	Attribute& operator=(const uint64_t value)
	{
		Set(value);
		return *this;
	}
	Attribute& operator=(const float value)
	{
		Set(value);
		return *this;
	}
	Attribute& operator=(const double value)
	{
		Set(value);
		return *this;
	}
	Attribute& operator=(const std::string& value)
	{
		Set(value);
		return *this;
	}

	int64_t GetSigned() const
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

	uint64_t GetUnsigned() const
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

	float GetFloat() const
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

	double GetDouble() const
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

	std::string GetString() const
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

	const uint16_t GetId() const		{ return m_id; }
	void SetId(const uint16_t value)	{ m_id = value; }

	const AttributeType GetType() const	{ return m_type; }
	void SetType(const AttributeType t)	{ m_type = t; }

	const std::string GetName() const	{ return m_name; }
	void SetName(const std::string& n)	{ m_name = n; }

	const bool GetIsDirty() const		{ return m_isDirty; }
	void SetIsDirty(bool dirty)			{ m_isDirty = dirty; }

public:
	static AttributeType TypeFromString(const std::string& type);

protected:
	uint16_t m_id;
	AttributeType m_type;
	std::string m_name;
	bool m_isDirty;

	union
	{
		int64_t s;
		uint64_t u;
	} m_value_int;
	float m_value_float;
	double m_value_double;
	std::string m_value_string;
};


class ObjectAttributes
{
public:
	ObjectAttributes() : m_cid(0) {}
	ObjectAttributes(const ObjectAttributes& props);
	~ObjectAttributes();

	void operator=(const ObjectAttributes& prop);

	//! Adds an attribute or changes the type of an existing one.
	//! \param name The name of the attribute.
	//! \param value The value of the attribute.
	//! \param id The id number of the attribute.
	//! \return New attribute, or nullptr on failure.
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, uint16_t id = -1);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, int64_t value, uint16_t id = -1);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, uint64_t value, uint16_t id = -1);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, float value, uint16_t id = -1);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, double value, uint16_t id = -1);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, const std::string& value, uint16_t id = -1);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, AttributeType type, const std::string& value, uint16_t id = -1);

	//! Returns an attribute.
	//! \param name The name of the attribute to get.
	//! \return A pointer to the attribute structure, or nullptr if the attribute was not found.
	std::shared_ptr<Attribute> Get(const std::string& name);

	//! Returns an attribute.
	//! \param id The id number of the attribute to get.
	//! \return A pointer to the attribute structure, or nullptr if the attribute was not found.
	std::shared_ptr<Attribute> Get(const uint16_t id);

	//! Returns an attribute.
	//! \param name The name of the attribute to get.
	//! \return A pointer to the attribute structure, or nullptr if the attribute was not found.
	std::shared_ptr<const Attribute> Get(const std::string& name) const;

	//! Returns an attribute.
	//! \param id The id number of the attribute to get.
	//! \return A pointer to the attribute structure, or nullptr if the attribute was not found.
	std::shared_ptr<const Attribute> Get(const uint16_t id) const;

	//! Returns the attribute map.
	//! \return The attribute map.
	std::map<uint16_t, std::shared_ptr<Attribute>>& GetMap()		{ return m_attributes; }

private:
	//! Assigns an id to the attribute.
	void assignId(const uint16_t id, Attribute& prop);

	//! Gets or creates an attribute.
	std::shared_ptr<Attribute> getOrCreateAttribute(const std::string& name, uint16_t id = -1);

	std::map<uint16_t, std::shared_ptr<Attribute>> m_attributes;
	uint16_t m_cid;
};

} // end namespace tdrp
