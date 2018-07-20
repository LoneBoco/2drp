#pragma once

#include "engine/common.h"
#include "engine/package/Package.h"
#include "engine/server/Server.h"

namespace tdrp::loader
{

class PackageLoader
{
public:
	PackageLoader() = delete;
	~PackageLoader() = delete;

	PackageLoader(const PackageLoader& other) = delete;
	PackageLoader(PackageLoader&& other) = delete;
	PackageLoader& operator=(const PackageLoader& other) = delete;
	PackageLoader& operator=(PackageLoader&& other) = delete;

	static std::pair<bool, std::shared_ptr<package::Package>> LoadIntoServer(server::Server& server, const std::string& name);
};

} // end namespace tdrp::loader
