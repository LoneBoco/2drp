#pragma once

#include <list>
#include <mutex>

#include <ZipFile.h>

#include "engine/common.h"
#include "engine/filesystem/common.h"

#include "engine/filesystem/watch/FileWatch.h"

namespace tdrp::fs
{

class File;
class FileSystem
{
private:
	enum class FileEntryType
	{
		SYSTEM = 0,
		ARCHIVE,
		ARCHIVEFILE,

		COUNT
	};

	struct FileEntry
	{
		FileEntry(FileEntryType type, const filesystem::path& file)
			: Type(type), File(file), Archive(nullptr)
		{
		}

		FileEntry(FileEntryType type, const filesystem::path& file, ZipArchive::Ptr& archive)
			: Type(type), File(file), Archive(archive)
		{
		}

		FileEntryType Type;
		ZipArchive::Ptr Archive;
		filesystem::path File;		// Full path + filename
	};

	using FileEntryPtr = std::unique_ptr<FileEntry, std::default_delete<FileEntry>>;

public:
	FileSystem() = default;
	~FileSystem() = default;

	FileSystem(const FileSystem& other) = delete;
	FileSystem(FileSystem&& other) = delete;
	FileSystem& operator=(const FileSystem& other) = delete;
	FileSystem& operator=(FileSystem&& other) = delete;

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

	//! Gets the first iterator for the directory we are watching.  Does not honor exclusion list.
	filesystem::directory_iterator GetFirstDirectoryIterator() const;

	//! Gets an iterator for the directory we are watching.  Does not honor exclusion list.
	std::list<filesystem::directory_iterator> GetDirectoryIterators() const;

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
	std::map<filesystem::path, FileEntryPtr> m_files;
	std::list<filesystem::path> m_directory_include;
	std::list<filesystem::path> m_directory_exclude;
	std::map<filesystem::path, ZipArchive::Ptr> m_archives;
	mutable std::mutex m_file_mutex;
};

} // end namespace tdrp::fs
