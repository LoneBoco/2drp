#pragma once

#include "engine/common.h"
#include "engine/filesystem/FileSystem.h"

namespace tdrp::package
{

class Package
{
public:
	Package(const std::string& name);
	~Package() = default;

	//! Gets the package name.
	inline const std::string GetName() const
	{
		return m_name;
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
	fs::FileSystem m_filesystem;
};

} // end namespace tdrp::package
