#include "engine/package/Package.h"

namespace tdrp::package
{

/////////////////////////////

Package::Package(const std::string& name)
	: m_name(name)
{
	m_filesystem = std::make_shared<fs::FileSystem>();
	m_filesystem->Bind(filesystem::path("packages") / name, "levels");
}

/////////////////////////////

} // end namespace tdrp::package
