#pragma once

#include "../nv12frame.h"
#include "utils.h"

#include <vulkan/vulkan.h>

namespace Impacto {
namespace Vulkan {

class VkNV12Frame : public NV12Frame {
  friend class Renderer;

 public:
  void Init(float width, float height) override;

  void Submit(const void* luma, int lumaStride, const void* cbcr,
              int cbcrStride) override;
  void Release() override;

 protected:
  VkTexture LumaImage{};
  VkTexture CbCrImage{};

 private:
  AllocatedBuffer StagingBuffer;
  void* MappedStagingBuffer;
};

}  // namespace Vulkan
}  // namespace Impacto