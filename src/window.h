#pragma once

#include <SDL.h>
#include "impacto.h"

namespace Impacto {

void WindowInit();
void WindowSetDimensions(int width, int height, int msaa, float renderScale);
int WindowGetScaledWidth();
int WindowGetScaledHeight();
void WindowUpdate();
void WindowShutdown();

extern SDL_Window* g_SDLWindow;
extern SDL_GLContext g_GLContext;
extern int g_WindowWidth;
extern int g_WindowHeight;
extern int g_MsaaCount;
extern float g_RenderScale;
extern bool g_FramebuffersNeedUpdate;
}  // namespace Impacto