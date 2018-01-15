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
	// TODO

	return nullptr;
}

/////////////////////////////

} // end namespace fs
} // end namespace tdrp
