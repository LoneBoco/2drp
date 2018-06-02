#include "BabyDI.Configuration.h"

#include "engine/common.h"

#include "client/render/Window.h"

#include <iostream>
#include <algorithm>

#ifdef __GNUC__
int main(int argc, char* argv[])
#elif _MSC_VER
#ifdef __cplusplus
extern "C"
#endif
int SDL_main(int argc, char* argv[])
#endif
{
  ConfigureBabyDI();

  tdrp::render::Window window("tdrp");
  window.EventLoop();

	SDL_Quit();

	return 0;
}
