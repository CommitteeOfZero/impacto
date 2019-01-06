#pragma once

#include <SDL.h>
#include "impacto.h"

#include "util.h"

namespace Impacto {
namespace Window {
enum GraphicsApi {
  GfxApi_GL,
  // Forces the use of a GLES driver (e.g. ANGLE on Windows)
  GfxApi_ForceNativeGLES,
  // Forces GLES context on desktop GL driver
  GfxApi_ForceDesktopGLES
};

void Init();
void SetDimensions(int width, int height, int msaa, float renderScale);
// Aspect ratio corrected viewport in window coordinates
RectF GetViewport();
// Aspect ratio corrected viewport in window coordinates scaled by RenderScale
RectF GetScaledViewport();
// Convert input events from window coordinates to viewport coordinates
void AdjustEventCoordinatesForNk(SDL_Event* ev);
void SwapRTs();
void Update();
void Draw();
void Shutdown();

extern SDL_Window* SDLWindow;
extern SDL_GLContext GLContext;

extern GraphicsApi GraphicsApiHint;
extern GraphicsApi ActualGraphicsApi;
extern bool GLDebug;

// Raw dimensions without aspect ratio correction. Only use for
// setting/determining resolution and drawing to window framebuffer!
extern int WindowWidth;
extern int WindowHeight;

// OS window dimensions * DpiScaleX/Y => WindowWidth/Height (real pixels)
// Always 1 unless high DPI support is SDL_WINDOW_ALLOW_HIGHDPI
extern float DpiScaleX;
extern float DpiScaleY;

extern int MsaaCount;
extern float RenderScale;

extern bool WindowDimensionsChanged;

// FBO of current render target for drawing onto
extern GLuint DrawRT;
// FBO of current render target for reading from
extern GLuint ReadRT;
// Texture associated with ReadRT
extern GLuint ReadRenderTexture;
}  // namespace Window
}  // namespace Impacto