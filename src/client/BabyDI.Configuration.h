#ifndef DOOMIO_BABYDI_CONFIGURATION_HPP
#define DOOMIO_BABYDI_CONFIGURATION_HPP

#include "BabyDI.hpp"

#include "client/game/Game.h"
#include "client/render/Render.h"
#include "client/render/Window.h"
#include "engine/filesystem/ProgramSettings.h"

static void ConfigureBabyDI() {
  BabyDI::Container container;
  container.Activate();

  auto game = new tdrp::Game();
  container.Bind<tdrp::Game>(game);

  auto settings = new tdrp::settings::ProgramSettings();
  settings->LoadFromFile("settings.ini");
  container.Bind<tdrp::settings::ProgramSettings>(settings);

  auto window = new tdrp::render::Window("tdrp");
  container.Bind<tdrp::render::Window>(window);

  auto render = new tdrp::render::Render();
  container.Bind<tdrp::render::Render>(render);
};

#endif //DOOMIO_BABYDI_CONFIGURATION_HPP
