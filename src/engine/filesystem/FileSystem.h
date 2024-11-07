#pragma once

#include <list>
#include <mutex>
#include <condition_variable>

#include "engine/common.h"
#include "engine/filesystem/common.h"

#include "engine/filesystem/watch/FileWatch.h"

#include <ZipFile.h>

namespace tdrp::fs
{

/// <summary>
/// Category of the file system.
/// </summary>
enum class FileCategory : uint8_t
{
	ASSETS = 0,
	CONFIG,
	ITEMS,
	LEVELS,
	UI,
	ACCOUNTS,

	COUNT
};
inline constexpr uint8_t FileCategoryCount = static_cast<uint8_t>(FileCategory::COUNT);

/// <summary>
/// Data about a file.
/// </summary>
struct FileData
{
	filesystem::path File;
	uint32_t CRC32 = 0;
	intmax_t TimeSinceEpoch = 0;
	uintmax_t FileSize = 0;
};

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
		FileEntry(FileEntryType type, const filesystem::path& file) : Type(type), File(file) {}
		FileEntry(FileEntryType type, const filesystem::path& file, ZipArchive::Ptr& archive) : Type(type), File(file), Archive(archive) {}

		FileEntryType Type;
		ZipArchive::Ptr Archive;
		filesystem::path File;		// Full path + filename
		std::filesystem::file_time_type ModifiedTime;
		uint32_t CRC32 = 0;
		size_t FileSize = 0;
	};

	using FileEntryPtr = std::unique_ptr<FileEntry, std::default_delete<FileEntry>>;

	struct DirectoryGroup
	{
		filesystem::path Directory;
		std::list<filesystem::path> ExcludedDirectories;
		std::map<const filesystem::path, FileEntryPtr> Files;
		std::map<const filesystem::path, FileEntryPtr> Archives;
	};

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
	void Bind(FileCategory category, const filesystem::path& directory, Args... exclude_dirs)
	{
		std::list<filesystem::path> exclude_list;
		buildExclusionList(exclude_list, exclude_dirs...);
		bind(category, directory, std::move(exclude_list));
	}

	//! Binds to a directory.
	//! \param directory The directory to bind to.
	//! \param exclude_dirs A list of directories to exclude.  Will be pattern matched like *exclude_dir*
	template <typename... Args>
	void BindFront(FileCategory category, const filesystem::path& directory, Args... exclude_dirs)
	{
		std::list<filesystem::path> exclude_list;
		buildExclusionList(exclude_list, exclude_dirs...);
		bind(category, directory, std::move(exclude_list), true);
	}

	//! Checks if the file exists in the filesystem.
	//! \return Successful if the file exists.
	bool HasFile(FileCategory category, const filesystem::path& file) const;

	//! Checks if the file exists in the filesystem.
	//! \return Successful if the file exists.
	bool HasFile(const filesystem::path& root_dir, const filesystem::path& file) const;

	//! Returns information about the file.
	//! \return Information about the file.
	FileData GetFileData(FileCategory category, const filesystem::path& file) const;

	//! Returns information about the file.
	//! \return Information about the file.
	FileData GetFileData(const filesystem::path& root_dir, const filesystem::path& file) const;

	//! Gets a file by name.
	//! \return A shared pointer to the file.
	std::shared_ptr<File> GetFile(FileCategory category, const filesystem::path& file) const;

	//! Gets a file by name.
	//! \return A shared pointer to the file.
	std::shared_ptr<File> GetFile(const filesystem::path& root_dir, const filesystem::path& file) const;

	//! Gets the full path to a file by name.
	//! \return The full path to a file.
	filesystem::path GetFilePath(FileCategory category, const filesystem::path& file) const;

	//! Gets the full path to a directory by name.
	filesystem::path GetDirectoryPath(FileCategory category, const filesystem::path& directory) const;

	//! Gets all the archive CRC32s.
	//! \return A map of the archive CRC32s.
	std::vector<FileData> GetArchiveInfo(FileCategory category) const;

	//! Gets all the archive CRC32s.
	//! \return A map of the archive CRC32s.
	std::vector<FileData> GetArchiveInfo(FileCategory category, const filesystem::path& root_dir) const;

	//! Gets a range of all files in a category.
	std::vector<std::filesystem::path> GetFiles(FileCategory category) const
	{
		std::vector<std::filesystem::path> result;
		auto& groups = m_categories[static_cast<uint8_t>(category)];
		for (const auto& group : groups)
			result.append_range(group.Files | std::views::keys);

		return result;
	}
	
	//! Gets a range of all directories in a category.
	auto GetDirectories(FileCategory category) const
	{
		const auto& groups = m_categories[static_cast<uint8_t>(category)];
		return groups | std::views::transform([](const auto& group) { return group.Directory; });
	}

	//! Gets the first iterator for the directory we are watching.  Does not honor exclusion list.
	// filesystem::directory_iterator GetFirstDirectoryIterator() const;

	//! Gets an iterator for the directory we are watching.  Does not honor exclusion list.
	// std::list<filesystem::directory_iterator> GetDirectoryIterators() const;

	//! Returns true if we are searching the filesystem.
	bool IsSearchingForFiles() const
	{
		return m_searching_files;
	}

	//! Blocks the thread until files have been fully searched.
	void WaitUntilFilesSearched()
	{
		if (!m_searching_files)
			return;

		std::unique_lock guard(m_file_mutex);
		m_searching_files_condition.wait(guard);
	}

	//! Checks for changes to the underlying OS filesystem.  Call this every so often.
	void Update();

private:
	void bind(FileCategory category, const filesystem::path& directory, std::list<filesystem::path>&& exclude_list, bool at_front = false);
	bool isExcluded(const std::list<filesystem::path>& exclude_list, const filesystem::path& directory);

private:
	void buildExclusionList(std::list<filesystem::path>& exclude_list) {}

	template <typename T, typename... Args>
	void buildExclusionList(std::list<filesystem::path>& exclude_list, T dir, Args... args)
	{
		exclude_list.push_back(dir);
		buildExclusionList(exclude_list, args...);
	}

private:
	watch::FileWatch m_watcher;
	std::atomic<bool> m_searching_files;
	std::condition_variable m_searching_files_condition;

	// Using list instead of vector as some compilers don't noexcept certain STL container member functions and can, down the line, cause issues with unique_ptr.
	std::array<std::list<DirectoryGroup>, FileCategoryCount> m_categories;

	mutable std::mutex m_file_mutex;
};

} // end namespace tdrp::fs
