#include "utils.h"

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace Impacto {
namespace Vulkan {

void CreateAllocator(VkPhysicalDevice physicalDevice, VkDevice device,
                     VkInstance instance) {
  VmaAllocatorCreateInfo allocatorInfo = {};
  VmaVulkanFunctions func = {};
  func.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  func.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
  allocatorInfo.pVulkanFunctions = &func;
  allocatorInfo.physicalDevice = physicalDevice;
  allocatorInfo.device = device;
  allocatorInfo.instance = instance;
  vmaCreateAllocator(&allocatorInfo, &Allocator);
}

AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage,
                             VmaMemoryUsage memoryUsage) {
  // allocate vertex buffer
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.pNext = nullptr;
  bufferInfo.size = allocSize;
  bufferInfo.usage = usage;
  VmaAllocationCreateInfo vmaallocInfo = {};
  vmaallocInfo.usage = memoryUsage;

  AllocatedBuffer newBuffer;

  // allocate the buffer
  vmaCreateBuffer(Allocator, &bufferInfo, &vmaallocInfo, &newBuffer.Buffer,
                  &newBuffer.Allocation, nullptr);

  return newBuffer;
}

void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function) {
  VkCommandBuffer cmd = MainUploadContext.CommandBuffer;
  VkCommandBufferBeginInfo cmdBeginInfo = {};
  cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  cmdBeginInfo.pNext = nullptr;
  cmdBeginInfo.pInheritanceInfo = nullptr;
  cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkResetFences(MainUploadContext.Device, 1, &MainUploadContext.UploadFence);

  vkBeginCommandBuffer(cmd, &cmdBeginInfo);
  function(cmd);
  vkEndCommandBuffer(cmd);

  VkSubmitInfo submit = {};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.pNext = nullptr;
  submit.waitSemaphoreCount = 0;
  submit.pWaitSemaphores = nullptr;
  submit.pWaitDstStageMask = nullptr;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &MainUploadContext.CommandBuffer;
  submit.signalSemaphoreCount = 0;
  submit.pSignalSemaphores = nullptr;
  vkQueueSubmit(MainUploadContext.GraphicsQueue, 1, &submit,
                MainUploadContext.UploadFence);

  vkWaitForFences(MainUploadContext.Device, 1, &MainUploadContext.UploadFence,
                  true, UINT64_MAX);
  vkResetFences(MainUploadContext.Device, 1, &MainUploadContext.UploadFence);

  vkResetCommandPool(MainUploadContext.Device, MainUploadContext.CommandPool,
                     0);
}

VkImageCreateInfo GetImageCreateInfo(VkFormat format, VkExtent3D extent) {
  VkImageCreateInfo dimgInfo = {};
  dimgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  dimgInfo.pNext = nullptr;
  dimgInfo.imageType = VK_IMAGE_TYPE_2D;
  dimgInfo.format = format;
  dimgInfo.extent = extent;
  dimgInfo.mipLevels = 1;
  dimgInfo.arrayLayers = 1;
  dimgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  dimgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  dimgInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  return dimgInfo;
}

}  // namespace Vulkan
}  // namespace Impacto
