#pragma once

#include <SDL.h>

namespace Impacto {

// For member variable generation
template <typename T>
class WindowTemplate {
 public:
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
  bool IsInit = false;

  int lastWidth = -1;
  int lastHeight = -1;
  int lastMsaa = 0;
  float lastRenderScale = 1.0f;
};

}  // namespace Impacto