#pragma once

#include "../window.h"

namespace Impacto::Bgfx {

class Window final : public BaseWindow {
 public:
  void Init() override;

  void SetDimensions(int width, int height, int msaa,
                     float renderScale) override;
  RectF GetViewport() override;
  RectF GetScaledViewport() override;

  void Update() override;

  void SwapRTs() override {}
  void Draw() override;

  void Shutdown() override;

 private:
  void UpdateDimensions() override;
};

}  // namespace Impacto::Bgfx
