#pragma once

#include "engine/common.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/scene/ObjectClass.h"

namespace tdrp::loader
{
	class PackageLoader;
}

namespace tdrp::package
{

class Package
{
	friend class loader::PackageLoader;

public:
	Package(const std::string& name);
	~Package() = default;

	//! Gets the package name.
	inline const std::string GetName() const
	{
		return m_name;
	}

	//! Gets the package description.
	inline const std::string GetDescription() const
	{
		return m_description;
	}

	//! Gets the base path for this package.
	inline const filesystem::path GetBasePath() const
	{
		return filesystem::path("packages") / m_name;
	}

	//! Gets the filesystem for this package.
	fs::FileSystem& GetFileSystem()
	{
		return m_filesystem;
	}

	//! Returns an object class from this package.
	std::shared_ptr<ObjectClass> GetObjectClass(const std::string objectClass);

	//! Returns the next SceneObject ID.
	uint32_t GetNextID()
	{
		return ++m_sceneobject_id;
	}

private:
	std::string m_name;
	std::string m_description;
	fs::FileSystem m_filesystem;
	std::map<std::string, std::shared_ptr<ObjectClass>> m_object_classes;
	uint32_t m_sceneobject_id;
};

} // end namespace tdrp::package
