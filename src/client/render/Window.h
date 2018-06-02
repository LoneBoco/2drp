#pragma once

#include <memory>

#include <SDL.h>
#include "BabyDI.hpp"

// TODO: this can be less lame
struct sdl_deleter {
  void operator()(SDL_Window* p) const { SDL_DestroyWindow(p); }
};

// Wrap an SDL window for BGFX
namespace tdrp {
  class Game;

  namespace render {
    class Window {
    public:
      Window(const char* title);
      Window(const Window& other) = delete;
      Window(Window&& other) = delete;

      ~Window();

      Window& operator=(const Window& other) = delete;

      void EventLoop();

    private:
      BabyDI::Injected<tdrp::Game> Game;

      std::unique_ptr<SDL_Window, sdl_deleter> m_window;
    };
  }
}
