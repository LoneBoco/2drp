#pragma once

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <typeinfo>
#include <any>

namespace tdrp
{

template <typename T>
class Resource
{
public:
	Resource() = delete;
	Resource(const Resource& other)
		: m_name(other.m_name), m_underlying(other.m_underlying)
	{}

	Resource(const Resource&& other)
		: m_name(std::move(other.m_name)), m_underlying(std::move(other.m_underlying))
	{}

	Resource(const std::string& name, T* underlying)
		: m_name(name), m_underlying(underlying)
	{}

	Resource(const std::string& name, std::shared_ptr<T>&& underlying)
		: m_name(name), m_underlying(std::move(underlying))
	{}

	virtual ~Resource() {}

	Resource& operator=(const Resource& other) = delete;
	Resource& operator=(Resource&& other) = delete;
	bool operator==(const Resource& other) = delete;

public:
	operator T* const()
	{
		return static_cast<T>(m_underlying.get());
	}

	operator const T* const () const
	{
		return static_cast<T>(m_underlying.get());
	}

	std::weak_ptr<T> Get() const
	{
		return std::weak_ptr<T>(m_underlying);
	}

	const std::string& GetName() const
	{
		return m_name;
	}

protected:
	std::shared_ptr<T> m_underlying;
	const std::string m_name;
};


// Manager.
class ResourceManager
{
	using ResourceMap = std::map<size_t, std::any>;
	//template <typename T>
	//using ResourceMap = std::map<size_t, Resource<T>>;

public:
	ResourceManager() = default;

	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager(const ResourceManager&& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;
	ResourceManager& operator=(ResourceManager&& other) = delete;
	bool operator==(const ResourceManager& other) = delete;

public:
	template <typename T>
	std::weak_ptr<T> Get(size_t id)
	{
		auto hash = typeid(T).hash_code();
		auto& r = m_resources[hash];

		auto i = r.find(id);
		if (i == r.end())
			return {};

		Resource<T>* p = std::any_cast<Resource<T>>(&i->second);
		if (p == nullptr) return {};

		return p->Get();
	}

	template <typename T>
	size_t FindId(const std::string& name)
	{
		auto hash = typeid(T).hash_code();
		auto& r = m_resources[hash];

		for (auto&& [key, value] : r)
		{
			if (Resource<T>* p = std::any_cast<Resource<T>>(&value))
			{
				if (p->GetName() == name)
					return key;
			}
		}

		return 0;
	}

	template <typename T>
	size_t Add(const std::string& name, T* resource)
	{
		return Add<T>(name, std::move(std::shared_ptr<T>(resource)));
	}

	template <typename T>
	size_t Add(const std::string& name, std::shared_ptr<T>&& resource)
	{
		auto hash = typeid(T).hash_code();
		auto& r = m_resources[hash];

		auto id = std::hash<std::string>()(name);
		auto a = std::make_any<Resource<T>>(name, std::move(resource));

		auto entry = std::make_pair(id, a);
		r.insert(entry);

		return id;
	}

private:
	std::unordered_map<size_t, ResourceMap> m_resources;
};

} // end namespace tdrp
