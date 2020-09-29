#include <future>
#include <fstream>

#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/File.h"

namespace tdrp::fs
{

bool isArchive(const filesystem::path& file)
{
	return (file.extension() == "zip" || file.extension() == "pak");
}

using ArchiveEntriesFuture = std::tuple<ZipArchive::Ptr, std::vector<filesystem::path>, uint32_t>;
std::future<ArchiveEntriesFuture> collectArchiveEntries(const filesystem::path file, ZipArchive::Ptr archive)
{
	auto f = std::async(std::launch::async, [file, archive]() -> ArchiveEntriesFuture
	{
		std::vector<filesystem::path> result;
		result.reserve(archive->GetEntriesCount());

		// Collect all full path names.
		for (size_t i = 0; i < archive->GetEntriesCount(); ++i)
		{
			result.push_back(archive->GetEntry(i)->GetFullName());
		}

		// Collect CRC32.
		uint32_t checksum = calculateCRC32(file);

		return std::make_tuple(archive, result, checksum);
	});

	return f;
}

/////////////////////////////

void FileSystem::bind(const filesystem::path& directory)
{
	std::map<filesystem::path, std::future<ArchiveEntriesFuture>> processingArchives;

	// We are starting our file search.
	m_searching_files = true;

	// Fill our filesystem with file information.
	for (const auto& file : filesystem::recursive_directory_iterator(directory))
	{
		// If it is not a regular file, abort.
		if (!filesystem::is_regular_file(file.status()))
			continue;

		const auto& path = file.path();

		// Check if the directory is in the exclusion list.
		if (isExcluded(path))
			continue;

		// Check if it is an archive file.
		if (isArchive(path))
		{
			auto archive = ZipFile::Open(path.string());
			if (archive)
			{
				auto archive_entry = std::make_unique<FileEntry>(FileEntryType::ARCHIVE, path, archive);
				archive_entry->FileSize = file.file_size();
				archive_entry->ModifiedTime = file.last_write_time();

				std::cout << "[ARCHIVE] " << path.filename() << std::endl;
				m_archives.insert(std::make_pair(path.filename(), std::move(archive_entry)));

				auto entry = std::make_unique<FileEntry>(FileEntryType::ARCHIVE, path);
				m_files.insert(std::make_pair(path.filename(), std::move(entry)));

				// Queue up archive file processing.
				processingArchives.insert(std::make_pair(path.filename(), collectArchiveEntries(path, archive)));
				continue;
			}
		}

		auto entry = std::make_unique<FileEntry>(FileEntryType::SYSTEM, path);
		m_files.insert(std::make_pair(path.filename(), std::move(entry)));
	}

	// Wait until all processing is done.
	std::for_each(processingArchives.begin(), processingArchives.end(), [](decltype(processingArchives)::value_type& pair)
	{
		pair.second.wait();
	});

	// We are done searching our file system.
	m_searching_files = false;

	// Merge all archive maps.
	std::map<filesystem::path, FileEntryPtr> finalFiles;
	std::for_each(processingArchives.begin(), processingArchives.end(), [this, &finalFiles](decltype(processingArchives)::value_type& pair)
	{
		auto& data = pair.second.get();
		auto& archive = std::get<0>(data);
		auto& files = std::get<1>(data);
		auto& crc32 = std::get<2>(data);

		// Update file entry with CRC32.
		auto& archive_name_iter = std::find_if(m_archives.begin(), m_archives.end(), [&archive](decltype(m_archives)::value_type& pair) { return pair.second->Archive == archive; });
		if (archive_name_iter != std::end(m_archives))
		{
			archive_name_iter->second->CRC32 = crc32;
		}

		// Create entries for the files.
		for (const auto& file : files)
		{
			auto entry = std::make_unique<FileEntry>(FileEntryType::ARCHIVEFILE, file, archive);
			finalFiles[file.filename()] = std::move(entry);
		}
	});

	// Swap our maps.  We will overwrite our archive file list with our loose file list.
	std::swap(m_files, finalFiles);

	// Finalize with moving our loose files into the map.
	std::copy(std::make_move_iterator(finalFiles.begin()), std::make_move_iterator(finalFiles.end()), std::inserter(m_files, m_files.end()));
	finalFiles.clear();

	m_directory_include.push_back(directory);
	m_watcher.Add(directory, [&](uint32_t watch_id, const filesystem::path& dir, const filesystem::path& file, watch::Event e)
	{
		if (e == watch::Event::Invalid || e == watch::Event::Modified)
			return;

		// Check if the directory is in the exclusion list.
		if (isExcluded(dir))
			return;

		// Used to remove an archive from the file system.
		auto removeArchive = [&]() -> void
		{
			// See if an archive with the same name was already loaded.
			auto archive_iter = m_archives.find(file);
			if (archive_iter != m_archives.end())
			{
				auto archive = archive_iter->second->Archive;

				// Remove all registered files that came from this archive.
				for (auto i = std::begin(m_files); i != std::end(m_files);)
				{
					if (i->second->Archive == archive)
						m_files.erase(i++);
					else
						++i;
				}
			}

			// Loop through our archives and make sure all files are represented in the file system.
			// The archive we removed may have been hiding older files in these archives.
			// We start from our found archive as precedence is by alphabetical order.  Later archives will never be overwritten.
			for (auto iter = std::make_reverse_iterator(archive_iter)++; iter != m_archives.rend(); ++iter)
			{
				auto& archive = iter->second->Archive;

				// Loop through all the files looking for ones that aren't in the file system (because they got removed).
				for (size_t i = 0; i < archive->GetEntriesCount(); ++i)
				{
					auto& entry = archive->GetEntry(i);
					auto& entry_file_name = entry->GetName();
					auto& entry_full_path = entry->GetFullName();

					// If we don't have this file, add it to the file system.
					if (m_files.find(entry_file_name) == std::end(m_files))
					{
						auto entry = std::make_unique<FileEntry>(FileEntryType::ARCHIVEFILE, entry_full_path, archive);
						m_files.insert(std::make_pair(entry_file_name, std::move(entry)));
					}
				}
			}

			// Remove the old archive now.
			m_archives.erase(archive_iter);
		};

		// Used to add a new archive to the existing file system.
		auto loadArchive = [&]() -> void
		{
			// Open up the new archive.
			auto archive = ZipFile::Open((dir / file).string());
			if (archive == nullptr)
				return;

			auto archive_entry = std::make_unique<FileEntry>(FileEntryType::ARCHIVE, dir / file, archive);
			archive_entry->CRC32 = calculateCRC32(dir / file);
			archive_entry->FileSize = filesystem::file_size(dir / file);
			archive_entry->ModifiedTime = filesystem::last_write_time(dir / file);

			std::cout << "[ARCHIVE] " << file << std::endl;
			m_archives.insert(std::make_pair(file, std::move(archive_entry)));

			// Collecting entries from our new version of this archive.
			for (auto i = 0; i < archive->GetEntriesCount(); ++i)
			{
				auto& entry = archive->GetEntry(i);
				auto& filename = entry->GetName();
				auto& fullpath = entry->GetFullName();

				// See if this file already exists.
				auto existing_entry = m_files.find(filename);

				// File does not already exist in the file system.  Add a new entry.
				if (existing_entry == m_files.end())
				{
					auto entry = std::make_unique<FileEntry>(FileEntryType::ARCHIVEFILE, fullpath, archive);
					m_files.insert(std::make_pair(filename, std::move(entry)));
				}
				// File DOES exist.
				else
				{
					// Only overwrite existing archive files and only if our archive comes later alphabetically.
					if (existing_entry->second->Type == FileEntryType::ARCHIVEFILE)
					{
						auto& existing_archive = existing_entry->second->Archive;
						auto& existing_archive_name = std::find_if(m_archives.begin(), m_archives.end(), [&existing_archive](decltype(m_archives)::value_type& pair) { return pair.second->Archive == existing_archive; });

						if (existing_archive_name == std::end(m_archives) || file > existing_archive_name->first)
						{
							existing_entry->second->File = fullpath;
							existing_entry->second->Archive = archive;
						}
					}
				}
			}
		};

		std::lock_guard<std::mutex> guard(m_file_mutex);
		auto iter = m_files.find(file);

		if (iter != m_files.end())
		{
			if (e == watch::Event::Add)
			{
				if (isArchive(file))
				{
					// This is bad.  We added another zip file somewhere with the same name as an existing one.
					// TODO: What to do?  Ignore?  Error?
					std::cout << "!! File watcher identified a duplicate archive file." << std::endl;
				}
				else
				{
					// Force it to be a SYSTEM type.  Loose files always take precedent over archived files.
					iter->second->Type = FileEntryType::SYSTEM;
					iter->second->Archive = nullptr;
					iter->second->File = dir / file;
				}
			}
			else if (e == watch::Event::Delete)
			{
				m_files.erase(iter);

				if (iter->second->Type == FileEntryType::ARCHIVE)
				{
					removeArchive();
				}
			}
			else if (e == watch::Event::Modified)
			{
				// Our archive got modified.  Remove it and reload it.
				if (iter->second->Type == FileEntryType::ARCHIVE)
				{
					removeArchive();
					loadArchive();
				}
			}
		}
		else
		{
			if (e == watch::Event::Add)
			{
				// Check if it is an archive file.
				if (isArchive(file))
				{
					loadArchive();
				}
				else
				{
					auto entry = std::make_unique<FileEntry>(FileEntryType::SYSTEM, dir / file);
					m_files.insert(std::make_pair(file, std::move(entry)));
				}
			}
		}
	});
}

bool FileSystem::HasFile(const filesystem::path& file) const
{
	if (filesystem::exists(file))
		return true;

	// Check if our file is saved in the file system list.
	{
		std::lock_guard<std::mutex> guard(m_file_mutex);

		/*
		if (std::any_of(m_directory_include.begin(), m_directory_include.end(), [&file](const filesystem::path& p) -> bool { return filesystem::exists(p / file); }))
			return true;
		*/

		auto iter = m_files.find(file);
		if (iter != m_files.end())
			return true;
	}

	return false;
}

FileData FileSystem::GetFileData(const filesystem::path& file) const
{
	FileData data;

	auto& f = GetFile(file);
	if (f)
	{
		data.CRC32 = f->Crc32();
		data.FileSize = f->Size();
		data.TimeSinceEpoch = f->ModifiedTime();
	}

	return data;
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
			switch (iter->second->Type)
			{
			case FileEntryType::SYSTEM:
				return std::make_shared<File>(iter->second->File);

			case FileEntryType::ARCHIVE:
				auto& archive = iter->second->Archive;
				for (size_t i = 0; i < archive->GetEntriesCount(); ++i)
				{
					auto entry = archive->GetEntry(i);
					if (entry->GetName() == file)
					{
						return std::make_shared<FileZip>(entry->GetFullName(), entry);
					}
				}
				break;
			}
		}
	}

	return nullptr;
}

std::vector<FileData> FileSystem::GetArchiveInfo() const
{
	std::vector<FileData> result;
	result.reserve(m_archives.size());

	for (const auto& [filename, entry] : m_archives)
	{
		auto modified_time = entry->ModifiedTime.time_since_epoch().count();
		result.push_back(FileData{ filename, entry->CRC32, modified_time, entry->FileSize });
	}

	return result;
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
	std::for_each(m_directory_include.begin(), m_directory_include.end(), [&directories](const decltype(m_directory_include)::value_type& p)
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
