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

	void Bind(const filesystem::path& directory);
	std::shared_ptr<File> GetFile(const filesystem::path& file) const;

	void Update();

private:
	watch::FileWatch m_watcher;
	std::map<filesystem::path, filesystem::path> m_files;
	mutable std::mutex m_file_mutex;
};

} // end namespace tdrp::fs
