#pragma once

#include <SDL.h>
#include "../util.h"

namespace Impacto {

enum class CursorType { Default, Pointer };

void SetWindowIcon(SDL_Window* window);
void InitCursors();
void RequestCursor(CursorType type);
void ApplyCursorForFrame();

enum GraphicsApi {
  GfxApi_GL,
  // Forces the use of a GLES driver (e.g. ANGLE on Windows)
  GfxApi_ForceNativeGLES,
  // Forces GLES context on desktop GL driver
  GfxApi_ForceDesktopGLES
};

class BaseWindow {
 public:
  virtual void Init() = 0;
  virtual void SetDimensions(int width, int height, int msaa,
                             float renderScale) = 0;
  // Aspect ratio corrected viewport in window coordinates
  virtual RectF GetViewport() = 0;
  // Aspect ratio corrected viewport in window coordinates scaled by RenderScale
  virtual RectF GetScaledViewport() = 0;
  virtual void SwapRTs() = 0;
  virtual void Update() = 0;
  virtual void Draw() = 0;
  virtual void Shutdown() = 0;

  SDL_Window* SDLWindow;

  // Raw dimensions without aspect ratio correction. Only use for
  // setting/determining resolution and drawing to window framebuffer!
  int WindowWidth = 0;
  int WindowHeight = 0;

  // OS window dimensions * DpiScaleX/Y => WindowWidth/Height (real pixels)
  // Always 1 unless high DPI support is SDL_WINDOW_ALLOW_HIGHDPI
  float DpiScaleX = 1.0f;
  float DpiScaleY = 1.0f;

  int MsaaCount = 4;
  float RenderScale = 1.0f;

  bool WindowDimensionsChanged;

 protected:
  virtual void UpdateDimensions() = 0;
  bool IsInit = false;

  int lastWidth = -1;
  int lastHeight = -1;
  int lastMsaa = 0;
  float lastRenderScale = 1.0f;
};

}  // namespace Impacto