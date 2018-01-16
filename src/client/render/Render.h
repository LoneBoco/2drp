#pragma once

#include "BabyDI.hpp"
#include "engine/common.h"

#include "client/game/Game.h"

namespace tdrp
{

class Render
{
public:
	Render();
	~Render() = default;

private:
  BabyDI::Injected<Game> game;

};

} // end namespace tdrp
