#pragma once

#include "../renderer.h"

#include <bgfx/bgfx.h>

namespace Impacto::Bgfx {

class Renderer final : public BaseRenderer {
 public:
  Renderer();
  ~Renderer() = default;
  void Init() override {}
  void Shutdown() override;

#ifndef IMPACTO_DISABLE_IMGUI
  void ImGuiBeginFrame() override;
#endif

  void BeginFrame() override;
  void BeginFrame2D() override;
  void EndFrame() override;

  uint32_t MapSpriteSheet(SpriteSheet const& sheet) override { return 0; }
  bool LoadSurf(int surfId, int archiveId, int fileId) override { return true; }
  void UnloadSurf(int surfId) override {}
  uint32_t SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                         int height) override {
    return 0;
  }

  int GetSpriteSheetImage(SpriteSheet const& sheet,
                          std::span<uint8_t> outBuffer) override {
    return 0;
  }
  void FreeTexture(uint32_t id) override {}
  YUVFrame* CreateYUVFrame(float width, float height) override {
    return nullptr;
  }
  NV12Frame* CreateNV12Frame(float width, float height) override {
    return nullptr;
  }

  void DrawSprite(Sprite const& sprite, CornersQuad const& dest,
                  glm::mat4 transformation, std::span<const glm::vec4, 4> tints,
                  glm::vec3 colorShift, bool inverted, bool disableBlend,
                  bool textureWrapRepeat) override {}

  void DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                        CornersQuad const& spriteDest,
                        CornersQuad const& maskDest, int alpha, int fadeRange,
                        glm::mat4 spriteTransformation,
                        glm::mat4 maskTransformation,
                        std::span<const glm::vec4, 4> tints, bool isInverted,
                        bool isSameTexture) override {}

  void DrawMaskedBinarySprite(Sprite const& sprite, Sprite const& mask,
                              CornersQuad const& spriteDest,
                              CornersQuad const& maskDest,
                              glm::mat4 spriteTransformation,
                              std::optional<glm::mat4> maskTransformation,
                              std::span<const glm::vec4, 4> tints,
                              bool isInverted) override {}

  void DrawMaskedSpriteOverlay(Sprite const& sprite, Sprite const& mask,
                               CornersQuad const& spriteDest,
                               CornersQuad const& maskDest, int alpha,
                               int fadeRange, glm::mat4 spriteTransformation,
                               glm::mat4 maskTransformation,
                               std::span<const glm::vec4, 4> tints,
                               bool isInverted, bool useMaskAlpha) override {}

  void DrawPrimitives(SpriteSheet const& sheet, SpriteSheet const* mask,
                      ShaderProgramType shaderType,
                      std::span<const VertexBufferSprites> vertices,
                      std::span<const uint16_t> indices,
                      glm::mat4 spriteTransformation,
                      glm::mat4 maskTransformation, bool inverted,
                      TopologyMode topology, bool textureWrapRepeat) override {}

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt) override {}

  void DrawEdgeDetectedSingleSheetFont(
      SpriteSheet const& sheet, SpriteSheet const* mask,
      std::span<const VertexBufferSprites> vertices,
      std::span<const uint16_t> indices, float differenceFactor,
      float intensityShift, float alphaShift, glm::vec2 renderScale,
      glm::mat4 spriteTransformation, glm::mat4 maskTransformation) override {}

  void DrawCHLCCMenuBackground(Sprite const& sprite, Sprite const& mask,
                               RectF const& dest, float alpha) override {}

  void DrawBlurredSprite(Sprite const& sprite, CornersQuad const& dest,
                         glm::mat4 transformation,
                         RendererBlurDirection blurDirection,
                         glm::vec4 tint) override {}

  void DrawMosaic(Sprite const& sprite, CornersQuad dest, float tileSize,
                  glm::mat4 transformation, glm::vec4 tint) override {}

  void DrawVideoTexture(YUVFrame const& frame, RectF const& dest,
                        glm::vec4 tint, bool alphaVideo) override {}
  void DrawVideoTexture(NV12Frame const& frame, RectF const& dest,
                        glm::vec4 tint, bool alphaVideo) override {}

  void DrawSubtitleGlyph(Sprite const& sprite, CornersQuad const& dest,
                         glm::mat4 transformation, glm::vec4 tint) override {}

  void CaptureScreencap(Sprite& sprite) override {}

  void SetFramebuffer(size_t buffer) override {}
  int GetFramebufferTexture(size_t buffer) override { return 0; }

  void EnableScissor() override {}
  void SetScissorRect(RectF const& rect) override {}
  void DisableScissor() override {}

  void SetStencilMode(StencilBufferMode mode) override {}
  void ClearStencilBuffer() override {}

  void SetBlendMode(RendererBlendMode blendMode) override {}

  void Clear(glm::vec4 color) override {}

 private:
  void Flush() override {}

  bgfx::DynamicIndexBufferHandle IndexBuffer;
  bgfx::DynamicVertexBufferHandle VertexBuffer;
  std::vector<uint16_t> Indices;
  std::vector<VertexBufferSprites> Vertices;

  bgfx::VertexLayout VertexBufferSpritesLayout;
};

}  // namespace Impacto::Bgfx
