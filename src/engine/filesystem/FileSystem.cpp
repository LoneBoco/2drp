#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/File.h"

namespace tdrp::fs
{

/////////////////////////////

void FileSystem::bind(const filesystem::path& directory)
{
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

		// Check if it is an archive file.
		if (path.extension() == "zip" || path.extension() == "pak")
		{
			auto archive = ZipFile::Open(path.string());
			if (archive)
			{
				std::cout << "[ARCHIVE] " << path.filename() << std::endl;

				m_archives.insert(std::make_pair(path.filename(), archive));
				continue;
			}
		}

		m_files.insert(std::make_pair(path.filename(), path.parent_path()));
	}

	m_directory_include.push_back(directory);
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
	// TODO: File list should be cached at some point.
	for (auto& [key, archive] : m_archives)
	{
		for (size_t i = 0; i < archive->GetEntriesCount(); ++i)
		{
			auto entry = archive->GetEntry(i);
			if (entry->GetName() == file)
			{
				return std::make_shared<FileZip>(entry->GetFullName(), entry);
			}
		}
	}

	return nullptr;
}

filesystem::directory_iterator FileSystem::GetFirstDirectoryIterator() const
{
	if (m_directory_include.empty())
		return filesystem::directory_iterator{};
	return filesystem::directory_iterator{ m_directory_include.front() };
}

std::list<filesystem::directory_iterator> FileSystem::GetDirectoryIterators() const
{
	std::list<filesystem::directory_iterator> directories;
	std::for_each(m_directory_include.begin(), m_directory_include.end(), [&directories](auto& p)
	{
		directories.push_back(filesystem::directory_iterator{ p });
	});
	return directories;
}

void FileSystem::Update()
{
	m_watcher.Update();
}

/////////////////////////////

} // end namespace tdrp::fs
