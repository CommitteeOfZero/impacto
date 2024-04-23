#pragma once

#include "../window.h"

namespace Impacto {
namespace LegacyOpenGL {

class LegacyGLWindow : public BaseWindow {
 public:
  void Init() override;
  void SetDimensions(int width, int height, int msaa,
                     float renderScale) override;
  RectF GetViewport() override;
  RectF GetScaledViewport() override;
  void SwapRTs() override;
  void Update() override;
  void Draw() override;
  void Shutdown() override;

#ifndef PLATFORM_DREAMCAST
  SDL_GLContext GLContext;
#endif

 private:
  void UpdateDimensions() override;
};

}  // namespace LegacyOpenGL
}  // namespace Impacto