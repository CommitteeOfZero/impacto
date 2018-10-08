#pragma once

#include <SDL.h>
#include "impacto.h"

namespace Impacto {

void WindowInit();
void WindowSetDimensions(int width, int height, int msaa, float renderScale);
int WindowGetScaledWidth();
int WindowGetScaledHeight();
void WindowSwapRTs();
void WindowUpdate();
void WindowDraw();
void WindowShutdown();

extern SDL_Window* g_SDLWindow;
extern SDL_GLContext g_GLContext;
extern int g_WindowWidth;
extern int g_WindowHeight;
extern int g_MsaaCount;
extern float g_RenderScale;
extern bool g_WindowDimensionsChanged;

// FBO of current render target for drawing onto
extern GLuint g_DrawRT;
// FBO of current render target for reading from
extern GLuint g_ReadRT;
// Texture associated with g_ReadRT
extern GLuint g_ReadRenderTexture;
}  // namespace Impacto