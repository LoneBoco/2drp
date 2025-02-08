#pragma once

#include <sstream>
#include <cstddef>
#include <string_view>
#include <variant>
#include <unordered_set>

#include "engine/common.h"

namespace tdrp
{

using AttributeVariant = std::variant<int64_t, double, std::string>;
inline constexpr AttributeID INVALID_ATTRIBUTE = 0xFFFF;

enum class AttributeType
{
	INVALID		= 0,
	INTEGER,
	DOUBLE,
	STRING,

	COUNT
};

const AttributeType GetAttributeType(const AttributeVariant& value);

///////////////////////////////////////////////////////////

class Attribute
{
public:
	Attribute() = default;
	Attribute(const AttributeID i) : ID{ i } {}
	Attribute(const Attribute& a) : ID{ a.ID }, Name{ a.Name }, Replicated{ a.Replicated }, Dirty{ a.Dirty }, NetworkUpdateRate{ a.NetworkUpdateRate }, m_value{ a.m_value }, m_update_cooldown{ a.m_update_cooldown } {}
	Attribute(Attribute&& o) noexcept;
	~Attribute() = default;

	Attribute& operator=(const Attribute& other);
	Attribute& operator=(Attribute&& other) noexcept;

	Attribute& Set(const Attribute& other);
	Attribute& Set(const AttributeVariant& value);
	Attribute& Set(const int64_t value);
	Attribute& Set(const double value);
	Attribute& Set(std::nullptr_t);
	Attribute& SetAsType(AttributeType type, const std::string& value);

	Attribute& operator=(const AttributeVariant& value);
	Attribute& operator=(const int64_t value);
	Attribute& operator=(const double value);

	bool operator==(const Attribute& other) const;

	auto operator<=>(is_numeric auto b)
	{
		return GetAs<decltype(b)>() <=> b;
	}
	auto operator<=>(const std::string& b)
	{
		return GetAs<std::string>().compare(b);
	}

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

	template <typename T>
	T GetAs() const
	{
		//static_assert(false, "Attribute::GetAs must be used with a number or a string.");
		assert(false);
		return {};
	}

	template <>
	std::string GetAs<std::string>() const
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

	// Non-const because RmlUi is dumb and requires non-const.
	template <is_numeric T>
	T GetAs()
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

	// Non-const because RmlUi is dumb and requires non-const.
	template <typename T>
	T GetAs()
	{
		assert(false);
		return {};
	}

	// Non-const because RmlUi is dumb and requires non-const.
	template <>
	std::string GetAs<std::string>()
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

	const AttributeType GetType() const;

	//! Sets the update cooldown to the network update rate.
	void SetUpdateCooldown()
	{
		m_update_cooldown = NetworkUpdateRate;
	}

	//! Updates the cooldown by the given time.
	void UpdateCooldown(std::chrono::milliseconds dt)
	{
		if (m_update_cooldown > dt)
			m_update_cooldown -= dt;
		else
			m_update_cooldown = 0ms;
	}

	//! Checks if we are ready for a network update.
	bool IsNetworkReadyForUpdate() const
	{
		return m_update_cooldown == 0ms;
	}

	//! Clears the dirty flag and notifies the dispatch if the dirty flag was true.
	//! Returns false if we were not dirty, true if we were.
	bool ProcessDirty()
	{
		if (!Dirty)
			return false;

		UpdateDispatch.Post(ID);
		Dirty = false;
		return true;
	}

public:
	static AttributeType TypeFromString(const std::string& type);
	std::string_view TypeAsString() const;
	friend std::ostream& operator<<(std::ostream& os, const Attribute& attribute)
	{
		return os << attribute.GetAs<std::string>();
	}

public:
	AttributeID ID = 0;
	std::string Name;
	bool Replicated = true;
	bool Dirty = false;
	std::chrono::milliseconds NetworkUpdateRate = 0ms;
	EventDispatcher<uint16_t> UpdateDispatch;

protected:
	AttributeVariant m_value;
	std::chrono::milliseconds m_update_cooldown = 0ms;
};

using AttributePtr = std::shared_ptr<Attribute>;

///////////////////////////////////////////////////////////

class ObjectAttributes
{
	friend class ObjectProperties;

public:
	using attribute_map = std::unordered_map<AttributeID, std::shared_ptr<Attribute>>;

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

	//! Returns if we have any dirty attributes.
	bool HasDirty() const
	{
		return std::any_of(m_attributes.begin(), m_attributes.end(), [](const auto& v) -> bool
		{
			return v.second->Dirty;
		});
	}

	//! Processes all dirty attributes and returns if any were dirty.
	bool ProcessAllDirty() const
	{
		bool result = false;
		for (auto& [id, attr] : m_attributes)
			result = attr->ProcessDirty() || result;
		return result;
	}

	//! Clears all the dirty flags and doesn't dispatch any events.
	void ClearAllDirty();

	EventDispatcher<uint16_t> DirtyUpdateDispatch;
	std::unordered_set<AttributeID> NetworkQueuedAttributes;

public:
	using value_type = Attribute;
	attribute_map::iterator begin() { return m_attributes.begin(); };
	attribute_map::iterator end() { return m_attributes.end(); }
	attribute_map::const_iterator begin() const { return m_attributes.begin(); }
	attribute_map::const_iterator end() const { return m_attributes.end(); }
	size_t size() const { return m_attributes.size(); }
	size_t max_size() const { return std::numeric_limits<attribute_map::difference_type>::max(); }

private:
	//! Assigns an id to the attribute.
	void assignId(const AttributeID id, Attribute& prop);

	//! Gets or creates an attribute.
	std::shared_ptr<Attribute> getOrCreateAttribute(const std::string& name, AttributeID id = -1);

	attribute_map m_attributes;
	AttributeID m_cid;
};

///////////////////////////////////////////////////////////

namespace attribute
{

inline constexpr auto dirty = std::views::filter([](const ObjectAttributes::attribute_map::value_type& a) { return a.second->Dirty; });

} // namespace attribute

} // end namespace tdrp
