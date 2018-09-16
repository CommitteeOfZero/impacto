#pragma once

#include <SDL.h>
#include "impacto.h"

namespace Impacto {

void WindowInit();
void WindowShutdown();

extern SDL_Window* g_SDLWindow;
extern SDL_GLContext g_GLContext;
extern int g_WindowWidth;
extern int g_WindowHeight;

extern nk_context* g_Nk;
}  // namespace Impacto