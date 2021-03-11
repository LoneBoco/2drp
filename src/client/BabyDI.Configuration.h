#pragma once

#include "BabyDI.hpp"

#include "client/game/Game.h"
#include "client/network/DownloadManager.h"
#include "client/render/Window.h"
#include "engine/filesystem/ProgramSettings.h"
#include "engine/resources/Resource.h"

static void ConfigureBabyDI()
{
	// PROVIDE(tdrp::settings::ProgramSettings, new tdrp::settings::ProgramSettings());
	PROVIDE(tdrp::Game, new tdrp::Game());
	PROVIDE(tdrp::ResourceManager, new tdrp::ResourceManager());
	PROVIDE(tdrp::DownloadManager, new tdrp::DownloadManager());
	PROVIDE(tdrp::render::Window, new tdrp::render::Window("tdrp"));
};
