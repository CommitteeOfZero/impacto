#include "yuvframe.h"

namespace Impacto {
namespace Vulkan {

void VkYUVFrame::Init(int width, int height) {
  Width = width;
  Height = height;

  VkDeviceSize imageSize = width * height;
  VkDeviceSize bufferSize = imageSize + 2 * ((width / 2) * (height / 2));
  VkFormat imageFormat = VK_FORMAT_R8_UNORM;

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

  auto lumaImageInfo = GetImageCreateInfo(imageFormat, lumaImageExtent);
  VmaAllocationCreateInfo dimgAllocinfo = {};
  dimgAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  vmaCreateImage(Allocator, &lumaImageInfo, &dimgAllocinfo,
                 &LumaImage.Image.Image, &LumaImage.Image.Allocation, nullptr);

  auto cbImageInfo = GetImageCreateInfo(imageFormat, cbCrImageExtent);
  vmaCreateImage(Allocator, &cbImageInfo, &dimgAllocinfo, &CbImage.Image.Image,
                 &CbImage.Image.Allocation, nullptr);

  auto crImageInfo = GetImageCreateInfo(imageFormat, cbCrImageExtent);
  vmaCreateImage(Allocator, &crImageInfo, &dimgAllocinfo, &CrImage.Image.Image,
                 &CrImage.Image.Allocation, nullptr);

  VkImageViewCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageInfo.pNext = nullptr;
  imageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageInfo.image = LumaImage.Image.Image;
  imageInfo.format = imageFormat;
  imageInfo.subresourceRange.baseMipLevel = 0;
  imageInfo.subresourceRange.levelCount = 1;
  imageInfo.subresourceRange.baseArrayLayer = 0;
  imageInfo.subresourceRange.layerCount = 1;
  imageInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  vkCreateImageView(MainUploadContext.Device, &imageInfo, nullptr,
                    &LumaImage.ImageView);

  imageInfo.image = CbImage.Image.Image;
  vkCreateImageView(MainUploadContext.Device, &imageInfo, nullptr,
                    &CbImage.ImageView);

  imageInfo.image = CrImage.Image.Image;
  vkCreateImageView(MainUploadContext.Device, &imageInfo, nullptr,
                    &CrImage.ImageView);
}

void VkYUVFrame::Submit(void* luma, void* cb, void* cr) {
  int cbOffset = Width * Height;
  int crOffset = (Width * Height) + ((Width / 2) * (Height / 2));

  uint8_t* mappedStagingBuffer = (uint8_t*)MappedStagingBuffer;

  memcpy(mappedStagingBuffer, luma, cbOffset);
  memcpy(mappedStagingBuffer + cbOffset, cb, (Width / 2) * (Height / 2));
  memcpy(mappedStagingBuffer + crOffset, cr, (Width / 2) * (Height / 2));

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

    imageBarrierToTransfer.image = CbImage.Image.Image;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrierToTransfer);

    copyRegion.bufferOffset = Width * Height;
    copyRegion.imageExtent = cbCrImageExtent;
    vkCmdCopyBufferToImage(cmd, StagingBuffer.Buffer, CbImage.Image.Image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &copyRegion);

    imageBarrierToReadable.image = CbImage.Image.Image;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &imageBarrierToReadable);

    imageBarrierToTransfer.image = CrImage.Image.Image;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrierToTransfer);

    copyRegion.bufferOffset = crOffset;
    vkCmdCopyBufferToImage(cmd, StagingBuffer.Buffer, CrImage.Image.Image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &copyRegion);

    imageBarrierToReadable.image = CrImage.Image.Image;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &imageBarrierToReadable);
  });
}

void VkYUVFrame::Release() {
  vmaUnmapMemory(Allocator, StagingBuffer.Allocation);
}

}  // namespace Vulkan
}  // namespace Impacto