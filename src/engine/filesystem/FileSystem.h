#pragma once

#include <filesystem>
#include <list>

#include "engine/common.h"

#include <ZipFile.h>

#ifdef _MSC_VER
namespace filesystem = std::experimental::filesystem;
#else
namespace filesystem = std::filesystem;
#endif

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
