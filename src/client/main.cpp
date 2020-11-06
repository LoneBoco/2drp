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
	auto settings = BabyDI::Get<tdrp::settings::ProgramSettings>();
	settings->LoadFromCommandLine(argc, argv);

	// Initialize the Game.
	auto game = BabyDI::Get<tdrp::Game>();
	game->Initialize();

	// Pull out the injected window and start the event loop.
	auto window = BabyDI::Get<tdrp::render::Window>();
	window->EventLoop();

	return 0;
}
