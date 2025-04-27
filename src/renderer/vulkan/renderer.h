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

  void DrawSprite(Sprite const& sprite, CornersQuad const& dest,
                  const std::array<glm::vec4, 4>& tints, float angle = 0.0f,
                  bool inverted = false) override;

  void DrawVertices(SpriteSheet const& sheet,
                    std::span<const glm::vec2> sheetPositions,
                    std::span<const glm::vec2> displayPositions, int width,
                    int height, glm::vec4 tint = glm::vec4(1.0),
                    bool inverted = false, bool disableBlend = false) override;

  void DrawRect(RectF const& dest, glm::vec4 color,
                float angle = 0.0f) override;

  void DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, bool isInverted = false,
                        bool isSameTexture = false) override;

  void DrawMaskedSpriteOffset(const Sprite& sprite, const Sprite& mask,
                              glm::vec2 pos, glm::vec2 origin, int alpha,
                              int fadeRange, glm::vec4 tint = glm::vec4(1.0f),
                              glm::vec2 scale = glm::vec2(1.0),
                              float angle = 0.0f, bool spriteInverted = false,
                              bool maskInverted = false,
                              bool isSameTexture = false) override;

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt) override;

  void DrawMaskedSpriteOverlay(Sprite const& sprite, Sprite const& mask,
                               RectF const& dest, glm::vec4 tint, int alpha,
                               int fadeRange, bool isInverted, float angle,
                               bool useMaskAlpha);

  void DrawCHLCCMenuBackground(const Sprite& sprite, const Sprite& mask,
                               const RectF& dest, float alpha) override;

  void DrawSprite3DRotated(Sprite const& sprite, RectF const& dest, float depth,
                           glm::vec2 vanishingPoint, bool stayInScreen,
                           glm::quat rot, glm::vec4 tint = glm::vec4(1.0f),
                           bool inverted = false) override;
  void DrawRect3DRotated(RectF const& dest, float depth,
                         glm::vec2 vanishingPoint, bool stayInScreen,
                         glm::quat rot, glm::vec4 color) override;

  void DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                        int verticesCount, float* mvlVertices, int indicesCount,
                        uint16_t* mvlIndices, bool inverted, glm::vec4 tint,
                        glm::vec2 scale) override;

  void DrawVideoTexture(YUVFrame* tex, RectF const& dest,
                        glm::vec4 tint = glm::vec4(1.0), float angle = 0.0f,
                        bool alphaVideo = false) override;

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
  void Flush();

  void MakeQuad();

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

  Sprite RectSprite;

  RectF PreviousScissorRect;

  static VkVertexInputBindingDescription GetBindingDescription();

  static std::array<VkVertexInputAttributeDescription, 4>
  GetAttributeDescriptions();
};

inline Renderer* MainRendererInstance;

}  // namespace Vulkan
}  // namespace Impacto
