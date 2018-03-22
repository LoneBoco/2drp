#include "engine/package/Package.h"

namespace tdrp::package
{

/////////////////////////////

Package::Package(const std::string& name)
	: m_name(name)
{
	m_filesystem.Bind(filesystem::path("packages") / name, "levels");
}

std::shared_ptr<ObjectClass> Package::GetObjectClass(const std::string objectClass)
{
	auto iter = m_object_classes.find(objectClass);
	if (iter != m_object_classes.end())
		return iter->second;

	return nullptr;
}

/////////////////////////////

} // end namespace tdrp::package
