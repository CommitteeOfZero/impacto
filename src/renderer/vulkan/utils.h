#pragma once

#include <vulkan/vulkan.h>
#include <functional>
#include <flat_hash_map.hpp>
#include <vma/vk_mem_alloc.h>
#include "pipeline.h"

namespace Impacto {
namespace Vulkan {

int constexpr MAX_FRAMES_IN_FLIGHT = 2;

inline uint32_t CurrentFrameIndex = 0;
inline uint32_t CurrentImageIndex = 0;

inline Pipeline* CurrentPipeline = nullptr;

inline VkSampler Sampler;

inline PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;

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

typedef struct VkTexture {
  AllocatedImage Image;
  VkImageView ImageView;
  VkDescriptorSet Descriptor;
} VkTexture;

inline ska::flat_hash_map<uint32_t, VkTexture> Textures;

struct UploadContext {
  VkDevice Device;
  VkQueue GraphicsQueue;
  VkFence UploadFence;
  VkCommandPool CommandPool;
  alignas(16) VkCommandBuffer CommandBuffer;
};

inline VmaAllocator Allocator;
inline UploadContext MainUploadContext;

void CreateAllocator(VkPhysicalDevice physicalDevice, VkDevice device,
                     VkInstance instance);

AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage,
                             VmaMemoryUsage memoryUsage);

void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

VkImageCreateInfo GetImageCreateInfo(VkFormat format, VkExtent3D extent);

}  // namespace Vulkan
}  // namespace Impacto
