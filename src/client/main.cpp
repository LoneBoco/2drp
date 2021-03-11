#ifdef __GNUC__
#define SDL_MAIN_HANDLED
#endif

#include <iostream>
#include <algorithm>

#include "BabyDI.Configuration.h"

#include "engine/common.h"
#include "engine/resources/Resource.h"

#include "client/game/Game.h"
#include "client/render/Window.h"
#include "client/network/DownloadManager.h"


int main(int argc, char* argv[])
{
	// Load all settings first.
	PROVIDE(tdrp::settings::ProgramSettings, new tdrp::settings::ProgramSettings());
	auto settings = BabyDI::Get<tdrp::settings::ProgramSettings>();
	settings->LoadFromFile("settings.ini");
	settings->LoadFromCommandLine(argc, argv);

	// Configure our various managers.
	try
	{
		ConfigureBabyDI();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	// Initialize the Game.
	auto game = BabyDI::Get<tdrp::Game>();
	game->Initialize();

	// Pull out the injected window and start the event loop.
	auto window = BabyDI::Get<tdrp::render::Window>();
	window->EventLoop();

	return 0;
}
