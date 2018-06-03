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

  namespace settings {
    class ProgramSettings;
  }

  namespace render {
    class Window {
    public:
      Window(const char* title);
      Window(const Window& other) = delete;
      Window(Window&& other) = delete;

      ~Window();

      Window& operator=(const Window& other) = delete;

      int32_t GetWidth() const {
        int32_t w;
        SDL_GetWindowSize(m_window.get(), &w, nullptr);

        return w;
      }

      int32_t GetHeight() const {
        int32_t h;
        SDL_GetWindowSize(m_window.get(), nullptr, &h);

        return h;
      }

      SDL_Window* GetSDLWindow() const {
        return m_window.get();
      }

      void EventLoop();

    private:
      BabyDI::Injected<tdrp::Game> Game;
      BabyDI::Injected<tdrp::settings::ProgramSettings> Settings;

      std::unique_ptr<SDL_Window, sdl_deleter> m_window;
    };
  }
}
