#pragma once
#include "../../util.h"
#include "../windowtemplate.h"

namespace Impacto {
class WindowInterface;

namespace Vulkan {

class VulkanWindow : public WindowTemplate<VulkanWindow> {
  friend class Impacto::WindowInterface;

 public:
  void Init();
  void SetDimensions(int width, int height, int msaa, float renderScale);
  RectF GetViewport();
  RectF GetScaledViewport();
  void SwapRTs();
  void Update();
  void Draw();
  void Shutdown();

 private:
  void UpdateDimensions();
};

}  // namespace Vulkan
}  // namespace Impacto