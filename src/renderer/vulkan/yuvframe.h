#pragma once

#include "../yuvframe.h"
#include "utils.h"

#include <vulkan/vulkan.h>

namespace Impacto {
namespace Vulkan {

class VkYUVFrame : public YUVFrame {
  friend class Renderer;

 public:
  void Init(float width, float height) override;

  void Submit(void* luma, void* cb, void* cr) override;
  void Release() override;

 protected:
  VkTexture LumaImage{};
  VkTexture CbImage{};
  VkTexture CrImage{};

 private:
  AllocatedBuffer StagingBuffer;
  void* MappedStagingBuffer;
};

}  // namespace Vulkan
}  // namespace Impacto