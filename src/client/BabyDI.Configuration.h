#pragma once

#include "BabyDI.hpp"

#include "client/game/Game.h"
#include "client/network/DownloadManager.h"
#include "client/render/Window.h"
#include "engine/filesystem/ProgramSettings.h"
#include "engine/resources/Resource.h"

static void ConfigureBabyDI()
{
	BabyDI::Container container;
	container.Activate();

	auto game = new tdrp::Game();
	container.Bind<tdrp::Game>(game);

	auto settings = new tdrp::settings::ProgramSettings();
	settings->LoadFromFile("settings.ini");
	container.Bind<tdrp::settings::ProgramSettings>(settings);

	auto resources = new tdrp::ResourceManager();
	container.Bind<tdrp::ResourceManager>(resources);

	auto downloader = new tdrp::DownloadManager();
	container.Bind<tdrp::DownloadManager>(downloader);

	auto window = new tdrp::render::Window("tdrp");
	container.Bind<tdrp::render::Window>(window);
};
