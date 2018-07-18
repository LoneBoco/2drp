#ifdef __GNUC__
#define SDL_MAIN_HANDLED
#endif

#include "BabyDI.Configuration.h"

#include "engine/common.h"

#include "client/render/Window.h"

#include <iostream>
#include <algorithm>

#ifdef __GNUC__
int main(int argc, char* argv[])
{
  SDL_SetMainReady();
#elif _MSC_VER
#ifdef __cplusplus
extern "C"
#endif
int SDL_main(int argc, char* argv[])
{
#endif
	ConfigureBabyDI();

	// Pull out the injected window and start the event loop
	BabyDI::Injected<tdrp::render::Window> window;
	window->EventLoop();

	return 0;
}
