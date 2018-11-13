#pragma once

#include "BabyDI.hpp"

#include "client/game/Game.h"
#include "client/render/Window.h"
#include "engine/filesystem/ProgramSettings.h"

static void ConfigureBabyDI()
{
	BabyDI::Container container;
	container.Activate();

	auto game = new tdrp::Game();
	container.Bind<tdrp::Game>(game);

	auto settings = new tdrp::settings::ProgramSettings();
	settings->LoadFromFile("settings.ini");
	container.Bind<tdrp::settings::ProgramSettings>(settings);

	auto window = new tdrp::render::Window("tdrp");
	container.Bind<tdrp::render::Window>(window);
};
