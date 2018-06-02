#pragma once

#include "BabyDI.hpp"
#include "engine/common.h"

#include "client/game/Game.h"

#include "bgfx/bgfx.h"

namespace tdrp {
  namespace render {
    class Render {
    public:
	    Render();
      Render(const Render& other) = delete;
      Render(Render&& other) = delete;

	    ~Render();

    private:
	    BabyDI::Injected<Game> game;
    };
  }
}
