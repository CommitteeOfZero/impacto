#pragma once

#include "../window.h"

namespace Impacto {
namespace DirectX9 {

class DirectX9Window : public BaseWindow {
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

 private:
  void UpdateDimensions() override;
};

}  // namespace DirectX9
}  // namespace Impacto