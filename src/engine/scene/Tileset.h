#pragma once

#include "engine/common.h"

namespace tdrp::scene
{

struct Tileset
{
	Tileset() : TileDimensions(16, 16) {}

	std::string File;
	Vector2di TileDimensions;
};

} // end namespace tdrp::scene
