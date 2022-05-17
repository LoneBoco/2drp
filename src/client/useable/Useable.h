#pragma once

#include "engine/common.h"

#include "engine/script/Function.h"


namespace tdrp::useable
{

struct Useable
{
	SCRIPT_SETUP;
	SCRIPT_FUNCTION(OnUsed);

	Useable(const std::string& name, const std::string& image, const std::string& description)
		: Name{ name }, Image{ image }, Description{ description }
	{}

	std::string Name;
	std::string Image;
	std::string Description;
};

using UseablePtr = std::shared_ptr<Useable>;

} // end namespace tdrp:useable
