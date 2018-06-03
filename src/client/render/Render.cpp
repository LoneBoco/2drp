#include "client/render/Render.h"
#include "client/render/Window.h"

#include <SDL_syswm.h>
#include "bgfx/platform.h"

using namespace tdrp::render;

Render::Render() {
  // Configure bgfx to use platform window from SDL
  // TODO: Handle failure fail fast hehe
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);

  if (!SDL_GetWindowWMInfo(Window->GetSDLWindow(), &wmi)) {
    printf("SDL_GetWindowWMInfo failed: %s\n", SDL_GetError());
  }

  bgfx::PlatformData pd = {0};
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
  pd.ndt          = (void*)wmi.info.x11.display;
  pd.nwh          = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
  pd.nwh          = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
  pd.nwh          = wmi.info.win.window;
#elif BX_PLATFORM_STEAMLINK
  pd.ndt          = wmi.info.vivante.display;
  pd.nwh          = wmi.info.vivante.window;
#endif
  bgfx::setPlatformData(pd);

  // Initialize bgfx
  bgfx::init(
    bgfx::RendererType::Count, // Auto-select appropriate renderer for platform
    BGFX_PCI_ID_NONE,          // Auto-select appropriate graphics adapter
    0,
    nullptr,
    nullptr
  );

  bgfx::reset(Window->GetWidth(), Window->GetHeight(), BGFX_RESET_NONE);
}

Render::~Render() {
}
