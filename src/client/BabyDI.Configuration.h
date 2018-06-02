#ifndef DOOMIO_BABYDI_CONFIGURATION_HPP
#define DOOMIO_BABYDI_CONFIGURATION_HPP

#include "BabyDI.hpp"

#include "client/game/Game.h"
#include "client/render/Render.h"

static void ConfigureBabyDI() {
  BabyDI::Container container;
  container.Activate();

  auto game = new tdrp::Game();
  container.Bind<tdrp::Game>(game);

  auto render = new tdrp::render::Render();
  container.Bind<tdrp::render::Render>(render);
};

#endif //DOOMIO_BABYDI_CONFIGURATION_HPP
