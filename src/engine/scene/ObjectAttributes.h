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
	Attribute(Attribute&& o) noexcept : m_id(o.m_id), m_type(o.m_type), m_isDirty(false), m_value_int(o.m_value_int), m_value_float(o.m_value_float), m_value_double(o.m_value_double), m_value_string(std::move(o.m_value_string)) {}
	~Attribute() {}

	Attribute& operator=(const Attribute& other) = delete;
	Attribute& operator=(Attribute&& other) = delete;

	Attribute& Set(const int64_t value);
	Attribute& Set(const uint64_t value);
	Attribute& Set(const float value);
	Attribute& Set(const double value);
	Attribute& Set(const std::string& value);
	Attribute& SetAsType(const AttributeType type, const std::string& value);

	Attribute& operator=(const int64_t value);
	Attribute& operator=(const uint64_t value);
	Attribute& operator=(const float value);
	Attribute& operator=(const double value);
	Attribute& operator=(const std::string& value);

	int64_t GetSigned() const;
	uint64_t GetUnsigned() const;
	float GetFloat() const;
	double GetDouble() const;
	std::string GetString() const;

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

	class IteratorDirty
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = Attribute;
		using difference_type = int;
		using pointer = Attribute * ;
		using reference = Attribute & ;

	public:
		IteratorDirty(ObjectAttributes& attributes, std::map<uint16_t, std::shared_ptr<Attribute>>::iterator iterator)
		{
			m_begin = attributes.m_attributes.begin();
			m_end = attributes.m_attributes.end();
			m_iter = iterator;
		}
		IteratorDirty(IteratorDirty& other) : m_begin(other.m_begin), m_end(other.m_end), m_iter(other.m_iter) {}

	public:
		bool operator==(const IteratorDirty& other) const { return m_iter == other.m_iter; }
		bool operator!=(const IteratorDirty& other) const { return m_iter != other.m_iter; }
		reference operator*() { return *m_iter->second; }
		pointer operator->() { return m_iter->second.get(); }

		IteratorDirty operator++()
		{
			IteratorDirty i = *this;
			while (m_iter != m_end)
			{
				++m_iter;
				if (m_iter->second->GetIsDirty())
					break;
			}
			return i;
		}
		IteratorDirty operator--()
		{
			IteratorDirty i = *this;
			while (m_iter != m_begin)
			{
				--m_iter;
				if (m_iter->second->GetIsDirty())
					break;
			}
			return i;
		}

		IteratorDirty operator++(int)
		{
			while (m_iter != m_end)
			{
				++m_iter;
				if (m_iter->second->GetIsDirty())
					break;
			}
			return *this;
		}
		IteratorDirty operator--(int)
		{
			while (m_iter != m_begin)
			{
				--m_iter;
				if (m_iter->second->GetIsDirty())
					break;
			}
			return *this;
		}

	protected:
		std::map<uint16_t, std::shared_ptr<Attribute>>::iterator m_begin;
		std::map<uint16_t, std::shared_ptr<Attribute>>::iterator m_iter;
		std::map<uint16_t, std::shared_ptr<Attribute>>::iterator m_end;
	};

public:
	ObjectAttributes() : m_cid(0) {}
	~ObjectAttributes();

	ObjectAttributes(const ObjectAttributes& other);
	ObjectAttributes(ObjectAttributes&& other) = delete;
	ObjectAttributes& operator=(const ObjectAttributes& other);
	ObjectAttributes& operator=(ObjectAttributes&& other) = delete;

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
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, const AttributeType type, const std::string& value, uint16_t id = -1);

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

	//! Returns an iterator to iterate over dirty attributes.
	IteratorDirty GetDirtyBegin() { return IteratorDirty(*this, m_attributes.begin()); }
	IteratorDirty GetDirtyEnd() { return IteratorDirty(*this, m_attributes.end()); }

	//! Returns if we have any dirty attributes.
	bool HasDirty() const
	{
		return std::any_of(m_attributes.begin(), m_attributes.end(), [](const auto& v) -> bool
		{
			return v.second->GetIsDirty();
		});
	}

private:
	//! Assigns an id to the attribute.
	void assignId(const uint16_t id, Attribute& prop);

	//! Gets or creates an attribute.
	std::shared_ptr<Attribute> getOrCreateAttribute(const std::string& name, uint16_t id = -1);

	std::map<uint16_t, std::shared_ptr<Attribute>> m_attributes;
	uint16_t m_cid;

public:
	class Dirty
	{
	public:
		Dirty(ObjectAttributes& attributes) { m_attributes = std::shared_ptr<ObjectAttributes>(&attributes); }
		IteratorDirty begin() { return m_attributes->GetDirtyBegin(); }
		IteratorDirty end() { return m_attributes->GetDirtyEnd(); }

	protected:
		std::shared_ptr<ObjectAttributes> m_attributes;
	};
};

} // end namespace tdrp
