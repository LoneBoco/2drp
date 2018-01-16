#ifndef DOOMIO_BABYDI_CONFIGURATION_HPP
#define DOOMIO_BABYDI_CONFIGURATION_HPP

#include "BabyDI.hpp"

#include "client/game/Game.h"

static void ConfigureDI() {
  BabyDI::Container container;
  container.Activate();

  auto game = new tdrp::Game();
  container.Bind<tdrp::Game>(game);
};

#endif //DOOMIO_BABYDI_CONFIGURATION_HPP
