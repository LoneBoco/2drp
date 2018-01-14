#pragma once

#ifdef __GNUC__
#include <experimental/filesystem>
#elif _MSC_VER
#include <filesystem>
#endif

#if defined(_MSC_VER) || defined(__GNUC__)
namespace filesystem = std::experimental::filesystem;
#else
namespace filesystem = std::filesystem;
#endif

#include <list>
#include "engine/common.h"

namespace tdrp
{
namespace fs
{

class File;
class FileSystem
{
public:
	FileSystem() = default;
	~FileSystem() = default;

	std::shared_ptr<File> GetFile(const filesystem::path& file) const;

	int32_t LoadZipFiles(const filesystem::path& directory);

private:
	std::list<std::tuple<filesystem::path, ZipArchive::Ptr>> m_zip_files;
};

}
}
