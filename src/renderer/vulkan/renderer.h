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

struct MaskedNoAlphaPushConstants {
  glm::vec2 Alpha;
  VkBool32 IsInverted;
};

struct YUVFramePushConstants {
  VkBool32 IsAlpha;
};

struct CCBoxPushConstants {
  glm::vec4 CCBoxAlpha;
};

class Renderer : public BaseRenderer {
 public:
  void RecreateSwapChain();

  void Init() override;
  void Shutdown() override;

#ifndef IMPACTO_DISABLE_IMGUI
  void ImGuiBeginFrame() override;
#endif

  void BeginFrame() override;
  void BeginFrame2D() override;
  void EndFrame() override;

  uint32_t SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                         int height) override;
  int GetSpriteSheetImage(SpriteSheet const& sheet,
                          std::span<uint8_t> outBuffer) override;
  void FreeTexture(uint32_t id) override;
  YUVFrame* CreateYUVFrame(float width, float height) override;

  void DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                  glm::mat4 transformation, std::span<const glm::vec4, 4> tints,
                  bool inverted, bool disableBlend) override;

  void DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                        const CornersQuad& spriteDest,
                        const CornersQuad& maskDest, int alpha, int fadeRange,
                        glm::mat4 spriteTransformation,
                        glm::mat4 maskTransformation,
                        std::span<const glm::vec4, 4> tints, bool isInverted,
                        bool isSameTexture) override;

  void DrawMaskedSpriteOverlay(const Sprite& sprite, const Sprite& mask,
                               const CornersQuad& spriteDest,
                               const CornersQuad& maskDest, int alpha,
                               int fadeRange, glm::mat4 spriteTransformation,
                               glm::mat4 maskTransformation,
                               std::span<const glm::vec4, 4> tints,
                               bool isInverted, bool useMaskAlpha) override;

  void DrawVertices(const SpriteSheet& sheet,
                    std::span<const VertexBufferSprites> vertices,
                    std::span<const uint16_t> indices, glm::mat4 transformation,
                    bool inverted) override;

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt) override;

  void DrawCHLCCMenuBackground(const Sprite& sprite, const Sprite& mask,
                               const RectF& dest, float alpha) override;

  void DrawVideoTexture(const YUVFrame& frame, const RectF& dest,
                        glm::vec4 tint, bool alphaVideo) override;

  void CaptureScreencap(Sprite& sprite) override;

  void SetFramebuffer(size_t buffer) override {};  // TODO: Implement
  int GetFramebufferTexture(size_t buffer) override {
    return 0;
  };  // TODO: Implement

  void EnableScissor() override;
  void SetScissorRect(RectF const& rect) override;
  void DisableScissor() override;

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
  void CreateColorAndDepthImage();
  void CreateCommandBuffer();
  void CreateSyncObjects();
  void CreateDescriptors();

  void CreateVertexBuffer();
  void CreateIndexBuffer();

  void CleanupSwapChain();

  void EnsureTextureBound(unsigned int texture);
  void EnsureMode(Pipeline* pipeline, bool flush = true);
  void Flush() override;

  void MakeQuad();

  glm::vec2 DesignToNDC(glm::vec2 designCoord) const override;

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

  AllocatedImage ColorImage;
  VkImageView ColorImageView;
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
  Pipeline* PipelineSpriteNoBlending;
  Pipeline* PipelineSpriteInverted;
  Pipeline* PipelineMaskedSprite;
  Pipeline* PipelineMaskedSpriteNoAlpha;
  Pipeline* PipelineYUVFrame;
  Pipeline* PipelineCCMessageBox;
  Pipeline* PipelineCHLCCMenuBackground;

  AllocatedBuffer VertexBufferAlloc;
  AllocatedBuffer IndexBufferAlloc;

  uint32_t CurrentTexture = 0;
  uint32_t NextTextureId = 1;

  VkYUVFrame* VideoFrameInternal;

  static int constexpr VertexBufferSize = 4096 * 4096;
  static int constexpr IndexBufferCount =
      VertexBufferSize / (4 * sizeof(VertexBufferSprites)) * 6;

  uint8_t* VertexBuffer;
  uint16_t* IndexBuffer;

  int VertexBufferFill = 0;
  int VertexBufferOffset = 0;
  int VertexBufferCount = 0;
  int IndexBufferFill = 0;
  int IndexBufferOffset = 0;

  RectF PreviousScissorRect;

  static VkVertexInputBindingDescription GetBindingDescription();

  static std::array<VkVertexInputAttributeDescription, 4>
  GetAttributeDescriptions();
};

inline Renderer* MainRendererInstance;

}  // namespace Vulkan
}  // namespace Impacto
