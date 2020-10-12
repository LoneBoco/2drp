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
	// Configure our various managers.
	ConfigureBabyDI();

	// Inject command line arguments into the settings.
	BabyDI::Injected<tdrp::settings::ProgramSettings> settings;
	settings->LoadFromCommandLine(argc, argv);

	// Initialize the Game.
	BabyDI::Injected<tdrp::Game> game;
	game->Initialize();

	// Pull out the injected window and start the event loop.
	BabyDI::Injected<tdrp::render::Window> window;
	window->EventLoop();

	return 0;
}
