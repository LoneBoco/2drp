#pragma once

#include "engine/common.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/scene/ObjectClass.h"
#include "engine/scene/Tileset.h"

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

	Package(const Package& other) = delete;
	Package(Package&& other) = delete;
	Package& operator=(const Package& other) = delete;
	Package& operator=(Package&& other) = delete;

	//! Gets the package name.
	inline const std::string& GetName() const
	{
		return m_name;
	}

	//! Gets the package description.
	inline const std::string& GetDescription() const
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

private:
	std::string m_name;
	std::string m_description;
	fs::FileSystem m_filesystem;
};

} // end namespace tdrp::package
