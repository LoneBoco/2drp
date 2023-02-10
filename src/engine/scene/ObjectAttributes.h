#pragma once

#include <sstream>
#include <cstddef>
#include <string_view>
#include <variant>

#include "engine/common.h"

namespace tdrp
{

using AttributeVariant = std::variant<int64_t, double, std::string>;
constexpr AttributeID INVALID_ATTRIBUTE = 0xFFFF;

enum class AttributeType
{
	INVALID		= 0,
	INTEGER,
	DOUBLE,
	STRING,

	COUNT
};

enum class AttributeDirty
{
	CLIENT,
	NETWORK,

	COUNT
};

const AttributeType GetAttributeType(const AttributeVariant& value);

///////////////////////////////////////////////////////////

class Attribute
{
public:
	struct Dirty
	{
	public:
		Dirty(std::chrono::milliseconds update_rate) : UpdateRate{ update_rate } {}

	public:
		bool GetIsDirty() const
		{
			return m_isDirty;
		}

		bool CanProcessDirty() const
		{
			return m_showDirty;
		}

		bool SetIsDirty(bool dirty)
		{
			bool was_dirty = m_isDirty;

			m_isDirty = dirty;
			m_showDirty = (m_updateCooldown == std::chrono::milliseconds::zero());

			m_updateCooldown = UpdateRate;

			return was_dirty;
		}

		void Update(const std::chrono::milliseconds& elapsed)
		{
			if (!m_isDirty) return;
			if (m_updateCooldown <= std::chrono::milliseconds::zero())
			{
				m_showDirty = true;
				m_updateCooldown = std::chrono::milliseconds::zero();
				return;
			}

			m_updateCooldown -= elapsed;
		}

	public:
		std::chrono::milliseconds UpdateRate = 0ms;
		EventDispatcher<uint16_t> UpdateDispatch;

	private:
		bool m_isDirty = false;
		bool m_showDirty = false;
		std::chrono::milliseconds m_updateCooldown = 0ms;
	};

public:
	Attribute() = default;
	Attribute(const AttributeID i, const AttributeType t) : ID{ i } {}
	Attribute(const Attribute& a) : ID{ a.ID }, ClientUpdate{ a.ClientUpdate }, NetworkUpdate { a.NetworkUpdate }, m_value{ a.m_value } {}
	Attribute(Attribute&& o) noexcept;
	~Attribute() = default;

	Attribute& operator=(const Attribute& other);
	Attribute& operator=(Attribute&& other) noexcept;

	Attribute& Set(const AttributeVariant& value);
	Attribute& Set(const Attribute& other);
	Attribute& Set(std::nullptr_t);
	Attribute& SetAsType(AttributeType type, const std::string& value);

	Attribute& operator=(const AttributeVariant& value);

	template <typename T>
	std::optional<T> Get() const
	{
		if (std::holds_alternative<T>(m_value))
			return std::get<T>(m_value);
		return std::nullopt;
	}

	template <is_numeric T>
	T GetAs() const
	{
		switch (GetType())
		{
		case AttributeType::INTEGER:
			return static_cast<T>(std::get<int64_t>(m_value));
		case AttributeType::DOUBLE:
			return static_cast<T>(std::get<double>(m_value));
		case AttributeType::STRING:
			{
				std::istringstream str(std::get<std::string>(m_value));
				T r{};
				str >> r;
				return r;
			}
		}

		return {};
	}

	template <is_string T>
	std::string GetAs() const
	{
		std::stringstream str;
		switch (GetType())
		{
		case AttributeType::STRING:
			return std::get<std::string>(m_value);
		case AttributeType::INTEGER:
			str << std::get<int64_t>(m_value);
			break;
		case AttributeType::DOUBLE:
			str << std::get<double>(m_value);
			break;
		}
		return str.str();
	}

	template <typename T>
	T GetAs() const
	{
		//static_assert(false, "Attribute::GetAs must be used with a number or a string.");
		assert(false);
		return {};
	}

	const AttributeType GetType() const;

	//! Checks if any dirty flag is set.
	bool IsAnyDirty() const
	{
		return ClientUpdate.GetIsDirty() || NetworkUpdate.GetIsDirty();
	}

	//! Checks if a dirty flag is set.
	bool IsDirty(AttributeDirty dirtyType) const
	{
		switch (dirtyType)
		{
		case AttributeDirty::CLIENT:
			return ClientUpdate.GetIsDirty();
		case AttributeDirty::NETWORK:
			return NetworkUpdate.GetIsDirty();
		}
		return false;
	}

	//! Sets the dirty flag of the attribute.
	bool SetDirty(bool dirty, AttributeDirty dirtyType)
	{
		switch (dirtyType)
		{
		case AttributeDirty::CLIENT:
			return ClientUpdate.SetIsDirty(dirty);
		case AttributeDirty::NETWORK:
			return NetworkUpdate.SetIsDirty(!Replicated ? false : dirty);
		}
		return false;
	}

	//! Sets all the dirty flags.
	bool SetAllDirty(bool dirty)
	{
		bool result = SetDirty(dirty, AttributeDirty::CLIENT);
		result |= SetDirty(dirty, AttributeDirty::NETWORK);
		return result;
	}

	//! Clears all the dirty flags and does not notify the dispatch.
	void ResetAllDirty()
	{
		ClientUpdate.SetIsDirty(false);
		NetworkUpdate.SetIsDirty(false);
	}

	//! Clears the dirty flag and notifies the dispatch if the dirty flag was true.
	//! Returns false if the dispatch cleared the flag, true if we cleared it, or null if we were not dirty.
	std::optional<bool> ProcessDirty(AttributeDirty dirtyType)
	{
		if (dirtyType == AttributeDirty::CLIENT)
		{
			if (!ClientUpdate.GetIsDirty() || !ClientUpdate.CanProcessDirty())
				return {};

			ClientUpdate.UpdateDispatch.Post(ID);
			return ClientUpdate.SetIsDirty(false);
		}
		else // AttributeDirty::NETWORK
		{
			if (!NetworkUpdate.GetIsDirty() || !NetworkUpdate.CanProcessDirty())
				return {};

			NetworkUpdate.UpdateDispatch.Post(ID);
			return NetworkUpdate.SetIsDirty(false);
		}

		return {};
	}

public:
	AttributeID ID = 0;
	std::string Name;
	bool Replicated = true;
	Dirty ClientUpdate{ 0ms };
	Dirty NetworkUpdate{ 15ms };

public:
	static AttributeType TypeFromString(const std::string& type);
	std::string_view TypeAsString() const;
	friend std::ostream& operator<<(std::ostream& os, const Attribute& attribute)
	{
		return os << attribute.GetAs<std::string>();
	}

protected:
	AttributeVariant m_value;
};

using AttributePtr = std::shared_ptr<Attribute>;

///////////////////////////////////////////////////////////

class ObjectAttributes
{
	friend class ObjectProperties;
	using attribute_map = std::unordered_map<AttributeID, std::shared_ptr<Attribute>>;

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

			// Find the first dirty attribute.
			while (m_iter != m_end && !m_iter->second->IsAnyDirty())
				++m_iter;
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
			while (m_iter != m_end && !m_iter->second->IsAnyDirty())
				++m_iter;

			return i;
		}

		IteratorDirty operator++(int)
		{
			if (m_iter == m_end)
				return *this;

			++m_iter;
			while (m_iter != m_end && !m_iter->second->IsAnyDirty())
				++m_iter;

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

	std::shared_ptr<Attribute> AddAttribute(const AttributePtr& a, AttributeID id = INVALID_ATTRIBUTE);
	std::shared_ptr<Attribute> AddAttribute(const std::string& name, AttributeID id = INVALID_ATTRIBUTE);
	std::shared_ptr<Attribute> AddAttribute(const std::string& name, AttributeType type, const std::string& value);

	template <typename T> requires is_numeric<T> || is_string<T>
	std::shared_ptr<Attribute> AddAttribute(const std::string& name, const T& value, AttributeID id = INVALID_ATTRIBUTE)
	{
		auto attr = getOrCreateAttribute(name, id);
		attr->Set(value);
		return attr;
	}

	std::shared_ptr<Attribute> Get(const std::string& name);
	std::shared_ptr<Attribute> Get(const AttributeID id);
	std::shared_ptr<const Attribute> Get(const std::string& name) const;
	std::shared_ptr<const Attribute> Get(const AttributeID id) const;

	std::shared_ptr<Attribute> GetOrCreate(const std::string& name);

	//! Returns the attribute map.
	//! \return The attribute map.
	std::unordered_map<AttributeID, std::shared_ptr<Attribute>>& GetMap()		{ return m_attributes; }

	//! Returns an iterator to iterate over dirty attributes.
	IteratorDirty GetDirtyBegin() { return IteratorDirty(*this, m_attributes.begin()); }
	IteratorDirty GetDirtyEnd() { return IteratorDirty(*this, m_attributes.end()); }

	//! Returns if we have any dirty attributes.
	bool HasDirty() const
	{
		return std::any_of(m_attributes.begin(), m_attributes.end(), [](const auto& v) -> bool
		{
			return v.second->IsAnyDirty();
		});
	}

	//! Returns if we have any dirty attributes.
	bool HasDirty(AttributeDirty dirtyType) const
	{
		return std::any_of(m_attributes.begin(), m_attributes.end(), [dirtyType](const auto& v) -> bool
		{
			return v.second->IsDirty(dirtyType);
		});
	}

	//! Clears all the dirty flags and doesn't dispatch any events.
	void ClearAllDirty();

	EventDispatcher<uint16_t> DirtyUpdateDispatch;

public:
	attribute_map::iterator begin() { return m_attributes.begin(); };
	attribute_map::iterator end() { return m_attributes.end(); }
	attribute_map::const_iterator begin() const { return m_attributes.begin(); }
	attribute_map::const_iterator end() const { return m_attributes.end(); }

private:
	//! Assigns an id to the attribute.
	void assignId(const AttributeID id, Attribute& prop);

	//! Gets or creates an attribute.
	std::shared_ptr<Attribute> getOrCreateAttribute(const std::string& name, AttributeID id = -1);

	std::unordered_map<AttributeID, std::shared_ptr<Attribute>> m_attributes;
	AttributeID m_cid;

public:
	class Dirty
	{
	public:
		Dirty(ObjectAttributes& attributes) : m_attributes{ &attributes } {}
		IteratorDirty begin() { return m_attributes->GetDirtyBegin(); }
		IteratorDirty end() { return m_attributes->GetDirtyEnd(); }

	protected:
		ObjectAttributes* m_attributes;
	};
};

} // end namespace tdrp
