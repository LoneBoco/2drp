#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/File.h"

namespace tdrp::fs
{

/////////////////////////////

void FileSystem::bind(const filesystem::path& directory)
{
	m_watcher.Add(directory, [&](uint32_t watch_id, const filesystem::path& dir, const filesystem::path& file, watch::Event e)
	{
		if (e == watch::Event::Invalid || e == watch::Event::Modified)
			return;

		// Check if the directory is in the exclusion list.
		if (isExcluded(dir))
			return;

		std::lock_guard<std::mutex> guard(m_file_mutex);
		auto iter = m_files.find(file);

		if (iter != m_files.end())
		{
			if (e == watch::Event::Add)
				iter->second = dir;
			if (e == watch::Event::Delete)
				m_files.erase(iter);
		}
		else
		{
			if (e == watch::Event::Add)
				m_files.insert(std::make_pair(file, dir));
		}
	});

	// Fill our filesystem with file information.
	for (auto& file : filesystem::recursive_directory_iterator(directory))
	{
		// If it is not a regular file, abort.
		if (!filesystem::is_regular_file(file.status()))
			continue;

		auto& path = file.path();

		// Check if the directory is in the exclusion list.
		if (isExcluded(path))
			continue;

		m_files.insert(std::make_pair(path.filename(), path.parent_path()));
	}
}

std::shared_ptr<File> FileSystem::GetFile(const filesystem::path& file) const
{
	// Check if the file exists in the native file system and file is a direct path.
	if (filesystem::exists(file))
	{
		auto f = std::make_shared<File>(file);
		return f;
	}

	// Check if the file exists in the native file system and file is a filename we want to find.
	{
		std::lock_guard<std::mutex> guard(m_file_mutex);
		auto iter = m_files.find(file);
		if (iter != m_files.end())
		{
			filesystem::path fullfile = iter->second / iter->first;
			if (filesystem::exists(fullfile))
			{
				auto f = std::make_shared<File>(fullfile);
				return f;
			}
		}
	}

	// Check if the file exists in our attached zip files.
	// TODO

	return nullptr;
}

void FileSystem::Update()
{
	m_watcher.Update();
}

/////////////////////////////

} // end namespace tdrp::fs
