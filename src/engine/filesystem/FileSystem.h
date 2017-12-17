#pragma once

#include <filesystem>

#include "engine/common.h"

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

private:
};

}
}
