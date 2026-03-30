#include "nv12frame.h"

namespace Impacto {
namespace Vulkan {

void VkNV12Frame::Init(float width, float height) {
  Width = width;
  Height = height;

  VkDeviceSize imageSize = (VkDeviceSize)(width * height);
  VkDeviceSize bufferSize =
      imageSize + 2 * (((VkDeviceSize)width / 2) * ((VkDeviceSize)height / 2));

  StagingBuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VMA_MEMORY_USAGE_CPU_ONLY);
  vmaMapMemory(Allocator, StagingBuffer.Allocation, &MappedStagingBuffer);

  VkExtent3D lumaImageExtent;
  lumaImageExtent.width = static_cast<uint32_t>(width);
  lumaImageExtent.height = static_cast<uint32_t>(height);
  lumaImageExtent.depth = 1;

  VkExtent3D cbCrImageExtent;
  cbCrImageExtent.width = static_cast<uint32_t>(width / 2);
  cbCrImageExtent.height = static_cast<uint32_t>(height / 2);
  cbCrImageExtent.depth = 1;

  auto lumaImageInfo =
      GetImageCreateInfo(VK_FORMAT_R8_UNORM, lumaImageExtent,
                         VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);
  VmaAllocationCreateInfo dimgAllocinfo = {};
  dimgAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  vmaCreateImage(Allocator, &lumaImageInfo, &dimgAllocinfo,
                 &LumaImage.Image.Image, &LumaImage.Image.Allocation, nullptr);
  auto cbcrImageInfo =
      GetImageCreateInfo(VK_FORMAT_R8G8_UNORM, cbCrImageExtent,
                         VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);
  vmaCreateImage(Allocator, &cbcrImageInfo, &dimgAllocinfo,
                 &CbCrImage.Image.Image, &CbCrImage.Image.Allocation, nullptr);

  VkImageViewCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageInfo.pNext = nullptr;
  imageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageInfo.image = LumaImage.Image.Image;
  imageInfo.format = VK_FORMAT_R8_UNORM;
  imageInfo.subresourceRange.baseMipLevel = 0;
  imageInfo.subresourceRange.levelCount = 1;
  imageInfo.subresourceRange.baseArrayLayer = 0;
  imageInfo.subresourceRange.layerCount = 1;
  imageInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  vkCreateImageView(MainUploadContext.Device, &imageInfo, nullptr,
                    &LumaImage.ImageView);

  imageInfo.image = CbCrImage.Image.Image;
  imageInfo.format = VK_FORMAT_R8G8_UNORM;
  vkCreateImageView(MainUploadContext.Device, &imageInfo, nullptr,
                    &CbCrImage.ImageView);
}

void VkNV12Frame::Submit(const void* luma, const void* cbcr) {
  int cbcrOffset = (int)(Width * Height);

  uint8_t* mappedStagingBuffer = (uint8_t*)MappedStagingBuffer;

  memcpy(mappedStagingBuffer, luma, cbcrOffset);
  memcpy(mappedStagingBuffer + cbcrOffset, cbcr,
         (size_t)((Width) * (Height / 2)));

  ImmediateSubmit([&](VkCommandBuffer cmd) {
    VkExtent3D lumaImageExtent;
    lumaImageExtent.width = static_cast<uint32_t>(Width);
    lumaImageExtent.height = static_cast<uint32_t>(Height);
    lumaImageExtent.depth = 1;
    VkExtent3D cbCrImageExtent;
    cbCrImageExtent.width = static_cast<uint32_t>(Width / 2);
    cbCrImageExtent.height = static_cast<uint32_t>(Height / 2);
    cbCrImageExtent.depth = 1;

    VkImageSubresourceRange range;
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    VkImageMemoryBarrier imageBarrierToTransfer = {};
    imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrierToTransfer.image = LumaImage.Image.Image;
    imageBarrierToTransfer.subresourceRange = range;
    imageBarrierToTransfer.srcAccessMask = 0;
    imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrierToTransfer);

    VkBufferImageCopy copyRegion = {};
    copyRegion.bufferOffset = 0;
    copyRegion.bufferRowLength = 0;
    copyRegion.bufferImageHeight = 0;
    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.mipLevel = 0;
    copyRegion.imageSubresource.baseArrayLayer = 0;
    copyRegion.imageSubresource.layerCount = 1;
    copyRegion.imageExtent = lumaImageExtent;
    vkCmdCopyBufferToImage(cmd, StagingBuffer.Buffer, LumaImage.Image.Image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &copyRegion);

    VkImageMemoryBarrier imageBarrierToReadable = imageBarrierToTransfer;
    imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &imageBarrierToReadable);

    imageBarrierToTransfer.image = CbCrImage.Image.Image;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrierToTransfer);

    copyRegion.bufferOffset = (VkDeviceSize)(Width * Height);
    copyRegion.imageExtent = cbCrImageExtent;
    vkCmdCopyBufferToImage(cmd, StagingBuffer.Buffer, CbCrImage.Image.Image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &copyRegion);

    imageBarrierToReadable.image = CbCrImage.Image.Image;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &imageBarrierToReadable);
  });
}

void VkNV12Frame::Release() {
  vmaUnmapMemory(Allocator, StagingBuffer.Allocation);
}

}  // namespace Vulkan
}  // namespace Impacto