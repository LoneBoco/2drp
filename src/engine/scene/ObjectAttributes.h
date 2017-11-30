#pragma once

#include <map>
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
		Attribute(AttributeType t = AttributeType::INVALID) : m_id(0), m_type(t), m_value_int(0), m_value_float(0.0f), m_value_double(0.0) {}
		Attribute(const Attribute& a);
		Attribute(const uint16_t i, const AttributeType t) : m_id(i), m_type(t), m_value_int(0), m_value_float(0.0f), m_value_double(0.0) {}
		~Attribute() {}

		void Set(const int64_t value)
		{
			m_value_string = "";
			m_value_int = static_cast<uint64_t>(value);
			m_type = AttributeType::SIGNED;
		}

		void Set(const uint64_t value)
		{
			m_value_string = "";
			m_value_int = value;
			m_type = AttributeType::UNSIGNED;
		}

		void Set(const float value)
		{
			m_value_string = "";
			m_value_float = value;
			m_type = AttributeType::FLOAT;
		}

		void Set(const double value)
		{
			m_value_string = "";
			m_value_double = value;
			m_type = AttributeType::DOUBLE;
		}

		void Set(const std::string& value)
		{
			m_value_string = value;
			m_type = AttributeType::STRING;
		}

		int64_t GetSigned() const
		{
			switch (m_type)
			{
				case AttributeType::SIGNED:
				case AttributeType::UNSIGNED:
					return m_value_int;
				case AttributeType::FLOAT:
					return static_cast<int64_t>(m_value_float);
				case AttributeType::DOUBLE:
					return static_cast<int64_t>(m_value_double);
				case AttributeType::STRING:
				{
					std::istringstream str(m_value_string.c_str());
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
				case AttributeType::UNSIGNED:
					return m_value_int;
				case AttributeType::FLOAT:
					return static_cast<uint64_t>(m_value_float);
				case AttributeType::DOUBLE:
					return static_cast<uint64_t>(m_value_double);
				case AttributeType::STRING:
				{
					std::istringstream str(m_value_string.c_str());
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
				return static_cast<float>(static_cast<int64_t>(m_value_int));
			case AttributeType::UNSIGNED:
				return static_cast<float>(m_value_int);
			case AttributeType::FLOAT:
				return m_value_float;
			case AttributeType::DOUBLE:
				return static_cast<float>(m_value_double);
			case AttributeType::STRING:
			{
				std::istringstream str(m_value_string.c_str());
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
					return static_cast<double>(static_cast<int64_t>(m_value_int));
				case AttributeType::UNSIGNED:
					return static_cast<double>(m_value_int);
				case AttributeType::FLOAT:
					return static_cast<double>(m_value_float);
				case AttributeType::DOUBLE:
					return m_value_double;
				case AttributeType::STRING:
				{
					std::istringstream str(m_value_string.c_str());
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
					str << static_cast<int64_t>(m_value_int);
					break;
				case AttributeType::UNSIGNED:
					str << m_value_int;
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

	protected:
		uint16_t m_id;
		AttributeType m_type;
		std::string m_name;

		uint64_t m_value_int;
		float m_value_float;
		double m_value_double;
		std::string m_value_string;
};


class ObjectAttributes
{
	public:
		ObjectAttributes() : m_cid(0) {}
		ObjectAttributes(ObjectAttributes& props);
		~ObjectAttributes();

		void operator=(ObjectAttributes& prop);

		//! Adds an attribute or changes the m_type of an existing one.
		//! \param name The name of the attribute.
		//! \param value The value of the attribute.
		//! \param m_id The m_id number of the attribute.
		//! \return New attribute, or nullptr on failure.
		Attribute* AddAttribute(const std::string& name, int32_t id = -1);
		Attribute* AddAttribute(const std::string& name, int64_t value, int32_t id = -1);
		Attribute* AddAttribute(const std::string& name, uint64_t value, int32_t id = -1);
		Attribute* AddAttribute(const std::string& name, float value, int32_t id = -1);
		Attribute* AddAttribute(const std::string& name, double value, int32_t id = -1);
		Attribute* AddAttribute(const std::string& name, const std::string& value, int32_t id = -1);

		//! Returns an attribute.
		//! \param name The name of the attribute to get.
		//! \return A pointer to the attribute structure, or 0 if the attribute was not found.
		Attribute* Get(const std::string& name) const;

		//! Returns an attribute.
		//! \param m_id The id number of the attribute to get.
		//! \return A pointer to the attribute structure, or 0 if the attribute was not found.
		Attribute* Get(const uint16_t id) const;

		//! Returns the attribute map.
		//! \return The attribute map.
		std::map<uint16_t, Attribute*>& GetMap()			{ return m_attributes; }

	private:
		//! Assigns an id to the attribute.
		void assignId(const int32_t id, Attribute* prop);

		//! Gets or creates an attribute.
		Attribute* getOrCreateAttribute(const std::string& name, int32_t id = -1);

		std::map<uint16_t, Attribute*> m_attributes;
		uint16_t m_cid;
};

} // end namespace tdrp
