#pragma once

#include "engine/common.h"


namespace tdrp::server
{
	class Server;
}
namespace tdrp::scene
{
	class Scene;
}
namespace tdrp::package
{
	class Package;
}


namespace tdrp
{

class Loader
{
private:
	Loader() = default;
	~Loader() = default;

	Loader(const Loader&) = delete;
	Loader(Loader&&) noexcept = delete;
	Loader& operator=(const Loader&) = delete;
	Loader& operator=(Loader&&) noexcept = delete;
	bool operator==(const Loader&) = delete;

public:
	static std::shared_ptr<tdrp::scene::Scene> CreateScene(server::Server& server, const filesystem::path& level);

public:
	static std::pair<bool, std::shared_ptr<package::Package>> LoadPackageIntoServer(server::Server& server, const std::string& name);
};

} // end namespace tdrp
