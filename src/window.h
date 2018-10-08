#pragma once

#include <SDL.h>
#include "impacto.h"

#include "util.h"

namespace Impacto {

void WindowInit();
void WindowSetDimensions(int width, int height, int msaa, float renderScale);
// Aspect ratio corrected viewport in window coordinates
RectF WindowGetViewport();
// Aspect ratio corrected viewport in window coordinates scaled by g_RenderScale
RectF WindowGetScaledViewport();
// Convert input events from window coordinates to viewport coordinates
void WindowAdjustEventCoordinates(SDL_Event* ev);
void WindowSwapRTs();
void WindowUpdate();
void WindowDraw();
void WindowShutdown();

extern SDL_Window* g_SDLWindow;
extern SDL_GLContext g_GLContext;

// Raw dimensions without aspect ratio correction. Only use for
// setting/determining resolution and drawing to window framebuffer!
extern int g_WindowWidth;
extern int g_WindowHeight;

extern int g_MsaaCount;
extern float g_RenderScale;
extern float g_DesignWidth;
extern float g_DesignHeight;
extern bool g_WindowDimensionsChanged;

// FBO of current render target for drawing onto
extern GLuint g_DrawRT;
// FBO of current render target for reading from
extern GLuint g_ReadRT;
// Texture associated with g_ReadRT
extern GLuint g_ReadRenderTexture;
}  // namespace Impacto