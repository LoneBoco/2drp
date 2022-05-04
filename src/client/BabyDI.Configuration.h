#pragma once

#include "BabyDI.hpp"

#include "engine/server/Server.h"
#include "engine/resources/Resource.h"

#include "client/game/Game.h"
#include "client/render/Window.h"

static void ConfigureBabyDI()
{
	// ProgramSettings handled in main.cpp so we can fully load them before we created anything.
	// PROVIDE(tdrp::settings::ProgramSettings, new tdrp::settings::ProgramSettings());

	PROVIDE(tdrp::server::Server, new tdrp::server::Server());
	PROVIDE(tdrp::ResourceManager, new tdrp::ResourceManager());

	PROVIDE(tdrp::Game, new tdrp::Game());
	PROVIDE(tdrp::render::Window, new tdrp::render::Window("tdrp"));
};
