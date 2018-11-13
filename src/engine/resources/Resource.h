#pragma once

#include <memory>
#include <string>
#include <functional>

namespace tdrp
{

/**
 * Base class to hide templated implementation for a resource.
 *
 * Resource and BaseResource should always be owned by ResourcePool.
 * For shared ownership, look at ResourceHandle.
 */
class BaseResource
{
public:
	BaseResource(const BaseResource& other) = delete;

	BaseResource(const std::string& name)
		: m_name(name)
	{};

	virtual ~BaseResource() {};

	const std::string& GetName() const
	{
		return m_name;
	};

protected:
	const virtual void* GetOpaquePointerToResource() const = 0;

	const std::string m_name;
};

template <typename T>
class ResourceHandle;

/**
 * Resource class holds an underlying resource type.
 */
template <typename T>
class Resource : public BaseResource
{
	friend ResourceHandle<T>;

public:
	Resource(const Resource<T>& other) = delete;

	Resource(const std::string& name, std::unique_ptr<T> underlying)
		: BaseResource(name), m_underlying(std::move(underlying))
	{};

	const void* GetOpaquePointerToResource() const override
	{
		return (void*)m_underlying.get();
	};

private:
	std::unique_ptr<T> m_underlying;
};

/**
 * Handle to a resource
 */
template <typename T>
class ResourceHandle
{
public:
	ResourceHandle() {};

	ResourceHandle(const std::shared_ptr< const Resource<T> >& resource)
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

	const std::shared_ptr< const Resource<T> >& GetResource() const
	{
		return m_resource;
	};

	bool operator==(const ResourceHandle& other) const
	{
		return m_resource == other.m_resource;
	};

	T* operator->() const
	{
		return m_resource->m_underlying.get();
	};

private:
	const std::shared_ptr<const Resource<T>> m_resource;
};

} // end namespace tdrp
