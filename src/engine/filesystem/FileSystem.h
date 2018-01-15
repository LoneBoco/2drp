#pragma once

#ifdef __GNUC__
#include <experimental/filesystem>
#elif _MSC_VER
#include <filesystem>
#endif

#ifdef _MSC_VER
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

private:
};

}
}
