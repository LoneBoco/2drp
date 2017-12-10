#include "engine/common.h"

#include <iostream>

#include <SDL.h>


struct sdl_deleter
{
	void operator()(SDL_Window* p) const { SDL_DestroyWindow(p); }
};

#ifdef __cplusplus
extern "C"
#endif
int SDL_main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
		return 1;
	}

	auto window = std::unique_ptr<SDL_Window, sdl_deleter>(SDL_CreateWindow("2drp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, ::SDL_WINDOW_SHOWN));
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
