#pragma once

#include <vulkan/vulkan.h>
#include <map>
#include <array>

#include "../renderer.h"
#include "utils.h"
#include "window.h"
#include "yuvframe.h"

namespace Impacto {
namespace Vulkan {

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool EnableValidationLayers = false;
#else
const bool EnableValidationLayers = true;
#endif

struct SpritePushConstants {
  VkBool32 IsInverted;
  VkBool32 IsSameTexture;
  glm::vec2 Alpha;
};

struct YUVFramePushConstants {
  VkBool32 IsAlpha;
};

struct CCBoxPushConstants {
  glm::vec4 CCBoxAlpha;
};

struct VertexBufferSprites {
  glm::vec2 Position;
  glm::vec2 UV;
  glm::vec4 Tint;
  glm::vec2 MaskUV;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(VertexBufferSprites);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 4>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(VertexBufferSprites, Position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(VertexBufferSprites, UV);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(VertexBufferSprites, Tint);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(VertexBufferSprites, MaskUV);

    return attributeDescriptions;
  }
};

class Renderer : public BaseRenderer {
 private:
  void CreateInstance();
  void SetupDebug();
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  void FindQueues();
  void CreateSurface();
  void CreateSwapChain();
  void CreateImageViews();
  void CreateRenderPass();
  void CreateFramebuffers();
  void CreateCommandPool();
  void CreateDepthImage();
  void CreateCommandBuffer();
  void CreateSyncObjects();
  void CreateDescriptors();

  void CreateVertexBuffer();
  void CreateIndexBuffer();

  void InitImpl() override;
  void ShutdownImpl() override;

  void NuklearInitImpl() override;
  void NuklearShutdownImpl() override;
  int NuklearHandleEventImpl(SDL_Event* ev) override;

  void BeginFrameImpl() override;
  void BeginFrame2DImpl() override;
  void EndFrameImpl() override;

  uint32_t SubmitTextureImpl(TexFmt format, uint8_t* buffer, int width,
                             int height) override;
  void FreeTextureImpl(uint32_t id) override;
  YUVFrame* CreateYUVFrameImpl(int width, int height) override;

  void DrawSpriteImpl(Sprite const& sprite, RectF const& dest, glm::vec4 tint,
                      float angle, bool inverted, bool isScreencap) override;
  void DrawRectImpl(RectF const& dest, glm::vec4 color, float angle) override;

  void DrawMaskedSpriteImpl(Sprite const& sprite, Sprite const& mask,
                            RectF const& dest, glm::vec4 tint, int alpha,
                            int fadeRange, bool isScreencap, bool isInverted,
                            bool isSameTexture) override;

  void DrawCCMessageBoxImpl(Sprite const& sprite, Sprite const& mask,
                            RectF const& dest, glm::vec4 tint, int alpha,
                            int fadeRange, float effectCt,
                            bool isScreencap) override;

  void DrawSprite3DRotatedImpl(Sprite const& sprite, RectF const& dest,
                               float depth, glm::vec2 vanishingPoint,
                               bool stayInScreen, glm::quat rot, glm::vec4 tint,
                               bool inverted) override;
  void DrawRect3DRotatedImpl(RectF const& dest, float depth,
                             glm::vec2 vanishingPoint, bool stayInScreen,
                             glm::quat rot, glm::vec4 color) override;

  void DrawCharacterMvlImpl(Sprite const& sprite, glm::vec2 topLeft,
                            int verticesCount, float* mvlVertices,
                            int indicesCount, uint16_t* mvlIndices,
                            bool inverted, glm::vec4 tint) override;

  void DrawVideoTextureImpl(YUVFrame* tex, RectF const& dest, glm::vec4 tint,
                            float angle, bool alphaVideo) override;

  void CaptureScreencapImpl(Sprite const& sprite) override;

  void EnableScissorImpl() override;
  void SetScissorRectImpl(RectF const& rect) override;
  void DisableScissorImpl() override;

  void EnsureTextureBound(unsigned int texture);
  void EnsureMode(Pipeline* pipeline, bool flush = true);
  void Flush();

  inline void MakeQuad();
  inline void QuadSetUV(RectF const& spriteBounds, float designWidth,
                        float designHeight, uintptr_t uvs, int stride);
  inline void QuadSetPosition(RectF const& transformedQuad, float angle,
                              uintptr_t positions, int stride);
  inline void QuadSetPosition3DRotated(RectF const& transformedQuad,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       uintptr_t positions, int stride);

  bool Drawing = false;

  VulkanWindow* VkWindow;

  VkDebugUtilsMessengerEXT DebugMessenger;

  VkInstance Instance;
  VkPhysicalDevice PhysicalDevice;
  VkDevice Device;
  VkQueueFamilies QueueIndices;
  VkQueue GraphicsQueue;
  VkQueue PresentQueue;

  VkSurfaceKHR Surface;

  VkSwapchainKHR SwapChain;
  std::vector<VkImage> SwapChainImages;
  VkFormat SwapChainImageFormat;
  VkExtent2D SwapChainExtent;
  std::vector<VkImageView> SwapChainImageViews;
  std::vector<VkFramebuffer> SwapChainFramebuffers;

  AllocatedImage DepthImage;
  VkImageView DepthImageView;

  VkRenderPass RenderPass;

  VkCommandPool CommandPool;
  VkCommandBuffer CommandBuffers[MAX_FRAMES_IN_FLIGHT];

  VkSemaphore ImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
  VkSemaphore RenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
  VkFence InFlightFences[MAX_FRAMES_IN_FLIGHT];

  VkDescriptorPool DescriptorPool;
  VkDescriptorSetLayout SingleTextureSetLayout;
  VkDescriptorSetLayout DoubleTextureSetLayout;
  VkDescriptorSetLayout TripleTextureSetLayout;

  Pipeline* PipelineSprite;
  Pipeline* PipelineSpriteInverted;
  Pipeline* PipelineMaskedSprite;
  Pipeline* PipelineYUVFrame;
  Pipeline* PipelineCCMessageBox;

  AllocatedBuffer VertexBufferAlloc;
  AllocatedBuffer IndexBufferAlloc;

  uint32_t CurrentTexture = 0;
  uint32_t NextTextureId = 1;

  VkYUVFrame* VideoFrameInternal;

  static int const VertexBufferSize = 4096 * 4096;
  static int const IndexBufferCount =
      VertexBufferSize / (4 * sizeof(VertexBufferSprites)) * 6;

  uint8_t* VertexBuffer;
  uint16_t* IndexBuffer;

  int VertexBufferFill = 0;
  int VertexBufferOffset = 0;
  int VertexBufferCount = 0;
  int IndexBufferFill = 0;
  int IndexBufferOffset = 0;

  Sprite RectSprite;

  RectF PreviousScissorRect;
};

}  // namespace Vulkan
}  // namespace Impacto
