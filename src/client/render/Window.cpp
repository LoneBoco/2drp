#include <stdio.h>
#include <cstdlib>


#include "engine/filesystem/ProgramSettings.h"
#include "client/render/Window.h"
#include "client/game/Game.h"

using namespace tdrp::render;

Window::Window(const char* title) {
  // TODO: SDL itself obviously shouldn't be initialized here
  // Initialize SDL and window with title
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "SDL_Init error: " << SDL_GetError() << std::endl;
    return;
  }

	std::atexit(SDL_Quit);

    int width  = Settings->GetAs<int>("window.width",  640);
  int height = Settings->GetAs<int>("window.height", 480);

  m_window = std::unique_ptr<SDL_Window, sdl_deleter>(SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, ::SDL_WINDOW_SHOWN | ::SDL_WINDOW_OPENGL));

  if (m_window == nullptr) {
    std::cout << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;

    return;
  }
}

Window::~Window() {
}

void Window::EventLoop() {
  // TODO: This probably shouldn't even be here..
  // A more basic window type would be useful for GUI windows and shit, so this should do way less
  bool quit = false;
  SDL_Event e;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT)
        quit = true;
    }

    Game->Update();

    SDL_Delay(10);
  }
}
