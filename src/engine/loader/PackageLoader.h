#pragma once

#include "engine/common.h"
#include "engine/package/Package.h"

namespace tdrp::loader
{

class PackageLoader
{
public:
	PackageLoader() = default;
	~PackageLoader() = default;

	static std::shared_ptr<package::Package> CreatePackage(const std::string& name);
};

} // end namespace tdrp::loader
