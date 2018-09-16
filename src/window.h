#pragma once

#include <SDL.h>

namespace Impacto {

void WindowInit();

extern SDL_Window* g_SDLWindow;
extern SDL_GLContext g_GLContext;
extern int g_WindowWidth;
extern int g_WindowHeight;
}  // namespace Impacto