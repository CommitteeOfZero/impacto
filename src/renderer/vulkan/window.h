#pragma once

#include "../window.h"

namespace Impacto {
namespace Vulkan {

class VulkanWindow : public BaseWindow {
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

}  // namespace Vulkan
}  // namespace Impacto