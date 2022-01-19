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

constexpr uint16_t INVALID_ATTRIBUTE = 0xFFFF;

class Attribute
{
public:
	Attribute(const AttributeType t = AttributeType::INVALID) : m_id(0), m_type(t), m_isDirty(false), m_value_float(0.0f), m_value_double(0.0) { m_value_int.s = 0; }
	Attribute(const uint16_t i, const AttributeType t) : m_id(i), m_type(t), m_isDirty(false), m_value_float(0.0f), m_value_double(0.0) { m_value_int.s = 0; }
	Attribute(const Attribute& a) : m_id(a.m_id), m_type(a.m_type), m_isDirty(false), m_value_int(a.m_value_int), m_value_float(a.m_value_float), m_value_double(a.m_value_double), m_value_string(a.m_value_string) {}
	Attribute(Attribute&& o) noexcept;
	~Attribute() {}

	Attribute& operator=(const Attribute& other);
	Attribute& operator=(Attribute&& other) noexcept;

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
	EventDispatcher<uint16_t> UpdateDispatch;

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
	friend class ObjectProperties;
	using attribute_map = std::map<uint16_t, std::shared_ptr<Attribute>>;

public:

	class IteratorDirty
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = Attribute;
		using difference_type = int;
		using pointer = Attribute * ;
		using reference = Attribute & ;

	public:
		IteratorDirty(ObjectAttributes& attributes, attribute_map::iterator iterator)
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
			IteratorDirty i{ *this };
			if (m_iter == m_end)
				return i;

			++m_iter;
			while (m_iter != m_end)
			{
				if (m_iter->second->GetIsDirty())
					break;
				++m_iter;
			}
			return i;
		}

		IteratorDirty operator++(int)
		{
			if (m_iter == m_end)
				return *this;

			++m_iter;
			while (m_iter != m_end)
			{
				if (m_iter->second->GetIsDirty())
					break;
				++m_iter;
			}
			return *this;
		}

	protected:
		attribute_map::iterator m_begin;
		attribute_map::iterator m_iter;
		attribute_map::iterator m_end;
	};

public:
	ObjectAttributes() : m_cid(0) {}
	~ObjectAttributes();

	ObjectAttributes(const ObjectAttributes& other);
	ObjectAttributes(ObjectAttributes&& other) noexcept;
	ObjectAttributes& operator=(const ObjectAttributes& other);
	ObjectAttributes& operator=(ObjectAttributes&& other) noexcept;

	//! Adds an attribute or changes the type of an existing one.
	//! \param name The name of the attribute.
	//! \param value The value of the attribute.
	//! \param id The id number of the attribute.
	//! \return New attribute, or nullptr on failure.
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, uint16_t id = INVALID_ATTRIBUTE);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, int64_t value, uint16_t id = INVALID_ATTRIBUTE);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, uint64_t value, uint16_t id = INVALID_ATTRIBUTE);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, float value, uint16_t id = INVALID_ATTRIBUTE);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, double value, uint16_t id = INVALID_ATTRIBUTE);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, const std::string& value, uint16_t id = INVALID_ATTRIBUTE);
	std::weak_ptr<Attribute> AddAttribute(const std::string& name, const AttributeType type, const std::string& value, uint16_t id = INVALID_ATTRIBUTE);

	std::shared_ptr<Attribute> Get(const std::string& name);
	std::shared_ptr<Attribute> Get(const uint16_t id);
	std::shared_ptr<const Attribute> Get(const std::string& name) const;
	std::shared_ptr<const Attribute> Get(const uint16_t id) const;

	std::shared_ptr<Attribute> GetOrCreate(const std::string& name);

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

	//! Clears the dirty flag and dispatches events for the attributes.
	void ClearDirty();

	EventDispatcher<uint16_t> DirtyUpdateDispatch;

public:
	attribute_map::iterator begin() { return m_attributes.begin(); };
	attribute_map::iterator end() { return m_attributes.end(); }
	attribute_map::const_iterator begin() const { return m_attributes.begin(); }
	attribute_map::const_iterator end() const { return m_attributes.end(); }

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
		Dirty(ObjectAttributes& attributes) { m_attributes = &attributes; }
		IteratorDirty begin() { return m_attributes->GetDirtyBegin(); }
		IteratorDirty end() { return m_attributes->GetDirtyEnd(); }

	protected:
		ObjectAttributes* m_attributes;
	};
};

} // end namespace tdrp
