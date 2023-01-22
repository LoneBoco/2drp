#ifdef __GNUC__
#define SDL_MAIN_HANDLED
#endif

#include <iostream>
#include <algorithm>

#include "BabyDI.Configuration.h"

#include "engine/common.h"
#include "engine/filesystem/ProgramSettings.h"
#include "engine/resources/Resource.h"
#include "engine/script/Script.h"

#include "client/game/Game.h"
#include "client/render/Window.h"


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

	// Create the Game and the Window.
	auto game = BabyDI::Get<tdrp::Game>();
	auto window = BabyDI::Get<tdrp::render::Window>();

	// Initialize the game and begin the event loop.
	game->Initialize();
	window->EventLoop();

	// Clean up in order.
	RELEASE(tdrp::Game);
	RELEASE(tdrp::ResourceManager);

	// Window last for SFML cleanup.
	// SFML will clean up OpenGL resources so we must make sure everything is destroyed before this point.
	RELEASE(tdrp::render::Window);

	// Script manager last so everything gets cleaned up.
	RELEASE(tdrp::script::ScriptManager);

	return 0;
}
