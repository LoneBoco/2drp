#pragma once

#include <cstdio>
#include <vector>
#include <cstring>
#include <limits>
#include <algorithm>

constexpr const unsigned int E_NONE = std::numeric_limits<unsigned int>::max();
constexpr const unsigned int E_AUTO = E_NONE - 1;

template <int i>
struct MetaIndex {
	static constexpr const int value = i;
};

template <bool v>
struct MetaFlag {
	static constexpr const int value = v;
};

template <typename T, typename I, typename invalidator>
static constexpr const int NextValueForCounter() {
	if constexpr (decltype(T::__meta_counter(I{}))::value == false) {
		return I::value;
	} else {
		return NextValueForCounter<T, MetaIndex<I::value + 1>, invalidator>();
	}
};

struct EnumEntry {
	unsigned int value;
	const char* name;

	constexpr EnumEntry(const unsigned int value, const char* name) :
		name(name), value(value) {};

	static const EnumEntry* Create(EnumEntry* (*initializer)()) {
		return initializer();
	}

	operator unsigned int() const {
		return value;
	}

	operator int() const {
		return value;
	}
};

template <typename T>
struct EnumEntryInitializer {
	EnumEntryInitializer(int value, const char* name) {
		auto enumEntry = new EnumEntry(value, name);

		T::_enum_meta.enumEntries.push_back(enumEntry);
	};
};

struct EnumMeta {
	std::vector<EnumEntry*> enumEntries;
};

template<typename T>
struct EnumValue {
	friend T;

private:
	constexpr EnumValue(unsigned int underlying) : value(underlying) {};

public:
	constexpr operator unsigned int() const {
		return value;
	};

	constexpr operator int() const {
		return value;
	};

	constexpr operator T() const {
		return *((T*)&value);
	};

	T* operator->() const {
		return (T*)&value;
	};

	uint32_t value;
};

#define ENUM_ENABLE(enumName) \
using _enum_myType = enumName; \
\
template <typename I> \
static constexpr MetaFlag<false> __meta_counter(I); \
\
static inline EnumMeta _enum_meta; \
\
unsigned int GetValueByName(const char* name) { \
	for (auto& enumEntry : _enum_meta.enumEntries) { \
		if (strcmp(enumEntry->name, name) == 0) { \
			return enumEntry->value; \
		} \
	} \
	\
	return E_NONE; \
} \
\
_enum_myType& operator=(const EnumValue<_enum_myType>& rhs) { \
	value = rhs.value; \
	\
	return *this; \
} \
\
constexpr bool operator==(const EnumValue<_enum_myType>& rhs) const { \
	return value == rhs.value; \
} \
\
constexpr bool operator!=(const EnumValue<_enum_myType>& rhs) const { \
	return value != rhs.value; \
} \
\
operator unsigned int() const { \
	return value; \
} \
\
const EnumEntry* GetEnumEntry() const { \
	return *std::find_if( \
		_enum_myType::_enum_meta.enumEntries.begin(), _enum_myType::_enum_meta.enumEntries.end(), \
		[&](const EnumEntry* e) { \
			return e->value == value; \
		} \
	); \
} \
\
const char* Name() const { \
	return GetEnumEntry()->name; \
} \
\
_enum_myType* operator->() { \
	return this; \
} \
\
unsigned int value;

#define ENUM(entryName) \
public: \
static constexpr const EnumValue<_enum_myType> entryName = EnumValue<_enum_myType>(NextValueForCounter<_enum_myType, MetaIndex<0>, MetaIndex<__LINE__>>()); \
static constexpr const MetaFlag<true> __meta_counter(MetaIndex<entryName.value>); \
private: \
static inline EnumEntryInitializer _initializer_##entryName = EnumEntryInitializer<_enum_myType>(entryName, #entryName); \
public:
