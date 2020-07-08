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
	Resource(const Resource& other) = delete;
	Resource(const Resource&& other) = delete;

	Resource(const std::string& name, std::unique_ptr<T> underlying)
		: m_name(name), m_underlying(std::move(underlying))
	{};

	Resource(const std::string& name, T* underlying)
		: m_name(name), m_underlying(underlying)
	{};

	virtual ~Resource() {};

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

	T* Get() const
	{
		return m_underlying.get();
	}

	const std::string& GetName() const
	{
		return m_name;
	};

protected:
	std::unique_ptr<T> m_underlying;
	const std::string m_name;
};


/**
 * Handle to a resource
 */
template <typename T>
class ResourceHandle
{
public:
	ResourceHandle() {};

	ResourceHandle(const std::shared_ptr<const Resource<T>>& resource)
		: m_resource(resource)
	{};

	ResourceHandle(const ResourceHandle<T>& other)
		: ResourceHandle(other.GetResource())
	{};

	ResourceHandle(ResourceHandle<T>&& other) noexcept
		: m_resource(other.GetResource())
	{
		other.m_resource.reset();
	};

	~ResourceHandle() noexcept {
	};

public:
	ResourceHandle<T>& operator=(const ResourceHandle<T>& other)
	{
		ResourceHandle<T> tmp(other);
		*this = std::move(tmp);

		return *this;
	};

	ResourceHandle<T>& operator=(const ResourceHandle<T>&& other)
	{
		if (this == &other)
			return *this;

		m_resource = other.m_resource;
		other.m_resource.reset();

		return *this;
	};

	bool operator==(const ResourceHandle& other) const
	{
		return m_resource == other.m_resource;
	};

	operator T* const()
	{
		return static_cast<T>(m_resource.Get());
	}

	operator const T* const () const
	{
		return static_cast<T>(m_resouce.Get());
	}

	T* operator->() const
	{
		return m_resource->Get();
	};

	bool Empty() const
	{
		return m_resource == nullptr;
	}

	const std::string& GetName() const
	{
		return m_resource->GetName();
	};

private:
	const std::shared_ptr<const Resource<T>> m_resource;
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
	ResourceHandle<T> Get(size_t id)
	{
		auto r = m_resources[typeid(T).hash_code()];
		auto i = r.find(id);
		if (i == r.end())
			return {};

		auto p = std::shared_ptr<Resource<T>>{ std::any_cast<Resource<T>>(i->second) };
		return ResourceHandle<T>(p);
	}

	template <typename T>
	size_t FindId(const std::string& name)
	{
		auto r = m_resources[typeid(T).hash_code()];

		for (auto&& [key, value] : r)
		{
			if (std::any_cast<Resource<T>>(value).GetName() == name)
				return key;
		}

		return 0;
	}

	template <typename T>
	size_t Add(const std::string& name, T* resource)
	{
		return Add<T>(name, std::unique_ptr<T>(resource));
	}

	template <typename T>
	size_t Add(const std::string& name, std::unique_ptr<T> resource)
	{
		auto r = m_resources[typeid(T).hash_code()];

		auto id = std::hash<std::string>()(name);
		std::any a = Resource<T>(name, resource);

		r.insert(std::make_pair(id, a));
		return id;
	}

private:
	std::unordered_map<size_t, ResourceMap> m_resources;
};

} // end namespace tdrp
