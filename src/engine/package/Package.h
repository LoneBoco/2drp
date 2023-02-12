#pragma once

#include "engine/common.h"
#include "engine/filesystem/FileSystem.h"
#include "engine/scene/ObjectClass.h"
#include "engine/scene/Tileset.h"

namespace tdrp
{
	class Loader;
}

namespace tdrp::package
{

class Package
{
	friend class tdrp::Loader;

public:
	Package(const std::string& name);
	~Package() = default;

	Package(const Package& other) = delete;
	Package(Package&& other) = delete;
	Package& operator=(const Package& other) = delete;
	Package& operator=(Package&& other) = delete;

	//! Gets the package name.
	const std::string& GetName() const
	{
		return m_name;
	}

	//! Gets the package logo.
	const std::string& GetLogo() const
	{
		return m_logo;
	}

	//! Gets the package version.
	const std::string& GetVersion() const
	{
		return m_version;
	}

	//! Gets the package description.
	const std::string& GetDescription() const
	{
		return m_description;
	}
	
	//! Gets the package starting scene.
	const std::string& GetStartingScene() const
	{
		return m_starting_scene;
	}
	
	//! Gets the package loading scene.
	const std::string& GetLoadingScene() const
	{
		return m_loading_scene;
	}

	//! Gets the base path for this package.
	const filesystem::path GetBasePath() const
	{
		return m_basepath;
	}

private:
	std::string m_name;
	std::string m_logo;
	std::string m_version;
	std::string m_description;
	std::string m_starting_scene;
	std::string m_loading_scene;
	filesystem::path m_basepath;
};

} // end namespace tdrp::package
