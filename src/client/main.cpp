#include "BabyDI.Configuration.h"

#include "engine/common.h"
#include "client/game/Game.h"

#include <iostream>
#include <algorithm>

#include <SDL.h>


struct sdl_deleter
{
	void operator()(SDL_Window* p) const { SDL_DestroyWindow(p); }
};

#ifdef __GNUC__
int main(int argc, char* argv[])
#elif _MSC_VER
#ifdef __cplusplus
extern "C"
#endif
int SDL_main(int argc, char* argv[])
#endif
{
	tdrp::Game Game;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
		return 1;
	}

	int width = Game.Settings.GetInt("window.width", 640);
	int height = Game.Settings.GetInt("window.height", 480);

	auto window = std::unique_ptr<SDL_Window, sdl_deleter>(SDL_CreateWindow("2drp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, ::SDL_WINDOW_SHOWN));
	if (window == nullptr)
	{
		std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = true;
		}
		SDL_Delay(10);
	}

	SDL_Quit();

	return 0;
}
