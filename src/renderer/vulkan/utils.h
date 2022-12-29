#pragma once

#include <vulkan/vulkan.h>
#include <functional>
#include "../../vendor/vma/vk_mem_alloc.h"

namespace Impacto {
namespace Vulkan {

struct VkQueueFamilies {
  uint32_t GraphicsQueueIdx;
  uint32_t PresentQueueIdx;
};

struct AllocatedBuffer {
  VkBuffer Buffer;
  VmaAllocation Allocation;
};

struct AllocatedImage {
  VkImage Image;
  VmaAllocation Allocation;
};

struct VkTexture {
  AllocatedImage Image;
  VkImageView ImageView;
  VkDescriptorSet Descriptor;
};

struct UploadContext {
  VkDevice Device;
  VkQueue GraphicsQueue;
  VkFence UploadFence;
  VkCommandPool CommandPool;
  VkCommandBuffer CommandBuffer;
};

extern VmaAllocator Allocator;
extern UploadContext MainUploadContext;

void CreateAllocator(VkPhysicalDevice physicalDevice, VkDevice device,
                     VkInstance instance);

AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage,
                             VmaMemoryUsage memoryUsage);

void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

VkImageCreateInfo GetImageCreateInfo(VkFormat format, VkExtent3D extent);

}  // namespace Vulkan
}  // namespace Impacto
