#pragma once

#include <glm/glm.hpp>

#include "../../3d/scene.h"
#include "../window.h"
#include "../renderer.h"

namespace Impacto {
namespace Vulkan {

class Scene3D : public IScene3D {
 public:
  Scene3D(VulkanWindow* window, VkDevice device, VkRenderPass renderPass,
          VkCommandBuffer* commandBuffers);

  void Init();
  void Shutdown();
  void Update(float dt);
  void Render();

 private:
  bool IsInit = false;

  VulkanWindow* Window;
  VkDevice Device;
  VkRenderPass RenderPass;
  VkCommandBuffer* CommandBuffers;
};

}  // namespace Vulkan
}  // namespace Impacto