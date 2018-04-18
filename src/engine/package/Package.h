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

	//! Returns the next SceneObject ID.
	const uint32_t GetNextID()
	{
		return ++m_sceneobject_id;
	}

	//! Gets a loaded tileset.
	std::shared_ptr<scene::Tileset> GetTileset(const std::string& tileset)
	{
		auto i = m_tilesets.find(tileset);
		if (i == m_tilesets.end())
			return std::shared_ptr<scene::Tileset>(nullptr);

		return i->second;
	}

	//! Returns an object class from this package.
	std::shared_ptr<ObjectClass> GetObjectClass(const std::string& objectClass);

private:
	std::string m_name;
	std::string m_description;
	fs::FileSystem m_filesystem;
	std::map<std::string, std::shared_ptr<ObjectClass>> m_object_classes;
	std::map<std::string, std::shared_ptr<scene::Tileset>> m_tilesets;
	uint32_t m_sceneobject_id;
};

} // end namespace tdrp::package
