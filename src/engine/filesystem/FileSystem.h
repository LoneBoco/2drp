#pragma once

#include <list>
#include <mutex>

#include "engine/common.h"
#include "engine/filesystem/common.h"

#include "engine/filesystem/watch/FileWatch.h"

namespace tdrp::fs
{

class File;
class FileSystem
{
public:
	FileSystem() = default;
	~FileSystem() = default;

	//! Binds to a directory.
	//! \param directory The directory to bind to.
	//! \param exclude_dirs A list of directories to exclude.  Will be pattern matched like *exclude_dir*
	template <typename... Args>
	void Bind(const filesystem::path& directory, Args... exclude_dirs)
	{
		buildExclusionList(exclude_dirs...);
		bind(directory);
	}

	//! Gets a file by name.
	//! \return A shared pointer to the file.
	std::shared_ptr<File> GetFile(const filesystem::path& file) const;

	//! Checks for changes to the underlying OS filesystem.  Call this every so often.
	void Update();

private:
	void bind(const filesystem::path& directory);
	inline bool isExcluded(const filesystem::path& directory)
	{
		// Check if this directory has been excluded.
		auto exclude = std::find_if(m_directory_exclude.begin(), m_directory_exclude.end(), [&](const auto& excluded) -> bool
		{
			if (directory.parent_path().string().find(excluded.string()) != std::string::npos)
				return true;
			return false;
		});
		if (exclude == m_directory_exclude.end())
			return false;
		return true;
	}

private:
	void buildExclusionList() {}

	template <typename T, typename... Args>
	void buildExclusionList(T dir, Args... args)
	{
		m_directory_exclude.push_back(dir);
		buildExclusionList(args...);
	}

private:
	watch::FileWatch m_watcher;
	std::map<filesystem::path, filesystem::path> m_files;
	std::list<filesystem::path> m_directory_exclude;
	mutable std::mutex m_file_mutex;
};

} // end namespace tdrp::fs
