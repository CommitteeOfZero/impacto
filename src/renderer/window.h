#pragma once

#include <SDL.h>
#include "../util.h"

namespace Impacto {

class IWindow {
 public:
  virtual void Init() = 0;
  virtual void SetDimensions(int width, int height, int msaa, float renderScale) = 0;
  // Aspect ratio corrected viewport in window coordinates
  virtual RectF GetViewport() = 0;
  // Aspect ratio corrected viewport in window coordinates scaled by RenderScale
  virtual RectF GetScaledViewport() = 0;
  // Convert input events from window coordinates to viewport coordinates
  virtual void AdjustEventCoordinatesForNk(SDL_Event* ev) = 0;
  virtual void SwapRTs() = 0;
  virtual void Update() = 0;
  virtual void Draw() = 0;
  virtual void Shutdown() = 0;

  SDL_Window* SDLWindow;

  // Raw dimensions without aspect ratio correction. Only use for
  // setting/determining resolution and drawing to window framebuffer!
  int WindowWidth;
  int WindowHeight;

  // OS window dimensions * DpiScaleX/Y => WindowWidth/Height (real pixels)
  // Always 1 unless high DPI support is SDL_WINDOW_ALLOW_HIGHDPI
  float DpiScaleX;
  float DpiScaleY;

  int MsaaCount;
  float RenderScale;

  bool WindowDimensionsChanged;

 private:
  virtual void UpdateDimensions() = 0;
};

}