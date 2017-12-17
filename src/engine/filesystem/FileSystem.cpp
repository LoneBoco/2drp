#include "engine/filesystem/FileSystem.h"
#include "engine/filesystem/File.h"

namespace tdrp
{
namespace fs
{

/////////////////////////////

std::shared_ptr<File> FileSystem::GetFile(const filesystem::path& file) const
{
	// Check if the file exists in the native file system.
	if (filesystem::exists(file))
	{
		auto f = std::make_shared<File>(file);
		return f;
	}

	// Check if the file exists in our attached zip files.
	for (auto& e : m_zip_files)
	{
		const filesystem::path& filename = std::get<0>(e);
		const ZipArchive::Ptr& zip = std::get<1>(e);

		auto entry = zip->GetEntry(file.generic_string());
		if (entry != nullptr)
		{
			auto f = std::make_shared<File>(file, entry);
			return f;
		}
	}

	return nullptr;
}

int32_t FileSystem::LoadZipFiles(const filesystem::path& directory)
{
	int32_t count = 0;
	for (auto& p : filesystem::recursive_directory_iterator(directory))
	{
		auto& path = p.path();

		// If it is not a regular file, abort.
		if (!filesystem::is_regular_file(p.status()))
			continue;

		// See if we have already loaded this zip file.
		auto duplicate = std::find_if(m_zip_files.begin(), m_zip_files.end(),
			[&path](auto& e) -> bool
			{
				auto&& filename = std::get<0>(e);
				if (path == filename)
					return true;
				return false;
			}
		);

		// Duplicate was found, abort.
		if (duplicate != m_zip_files.end())
			continue;

		++count;
		m_zip_files.push_back(std::make_tuple(path, ZipFile::Open(path.generic_string())));
		m_zip_files.sort([](std::tuple<filesystem::path, ZipArchive::Ptr>& a, std::tuple<filesystem::path, ZipArchive::Ptr>& b) -> bool
		{
			return std::get<0>(a).generic_string() < std::get<0>(b).generic_string();
		});
	}

	return count;
}

/////////////////////////////

} // end namespace fs
} // end namespace tdrp
