#pragma once

#include "../renderer.h"
#include "window.h"
#include "shader.h"
#include "yuvframe.h"

#include <d3d9.h>

namespace Impacto {
namespace DirectX9 {

class Renderer : public BaseRenderer {
 public:
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
                          std::span<uint8_t> outBuffer) override {
    // TODO implement
    return 0;
  }
  void FreeTexture(uint32_t id) override;
  YUVFrame* CreateYUVFrame(float width, float height) override;

  void DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                  glm::mat4 transformation, std::span<const glm::vec4, 4> tints,
                  bool inverted, bool disableBlend,
                  bool textureWrapRepeat) override;

  void DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                        const CornersQuad& spriteDest,
                        const CornersQuad& maskDest, int alpha, int fadeRange,
                        glm::mat4 spriteTransformation,
                        glm::mat4 maskTransformation,
                        std::span<const glm::vec4, 4> tints, bool isInverted,
                        bool isSameTexture);

  void DrawMaskedSpriteOverlay(const Sprite& sprite, const Sprite& mask,
                               const CornersQuad& spriteDest,
                               const CornersQuad& maskDest, int alpha,
                               int fadeRange, glm::mat4 spriteTransformation,
                               glm::mat4 maskTransformation,
                               std::span<const glm::vec4, 4> tints,
                               bool isInverted, bool useMaskAlpha) override;

  void DrawVertices(const SpriteSheet& sheet, const SpriteSheet* mask,
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

  void SetBlendMode(RendererBlendMode blendMode) override {}

 private:
  void EnsureSpaceAvailable(int vertices, int vertexSize, int indices);
  void EnsureTextureBound(uint32_t texture);
  void EnsureShader(Shader* shader, bool flush = true);
  void Flush() override;

  glm::vec2 DesignToNDC(glm::vec2 designCoord) const override;

  DirectX9Window* DXWindow;

  IDirect3D9* Interface;
  IDirect3DDevice9* Device;

  Shader* CurrentShader;

  Shader* ShaderSprite;
  Shader* ShaderSpriteInverted;
  Shader* ShaderMaskedSprite;
  Shader* ShaderMaskedSpriteNoAlpha;
  Shader* ShaderYUVFrame;
  Shader* ShaderCCMessageBox;
  Shader* ShaderCHLCCMenuBackground;

  uint32_t CurrentTexture = 0;
  uint32_t NextTextureId = 1;

  DX9YUVFrame* VideoFrameInternal;

  bool Drawing = false;

  static int constexpr VertexBufferSize = 1024 * 1024;
  static int constexpr IndexBufferCount =
      VertexBufferSize / (4 * sizeof(VertexBufferSprites)) * 6;

  uint8_t VertexBuffer[VertexBufferSize];
  IDirect3DVertexBuffer9* VertexBufferDevice;
  int VertexBufferFill = 0;
  uint16_t IndexBuffer[IndexBufferCount];
  IDirect3DIndexBuffer9* IndexBufferDevice;
  IDirect3DIndexBuffer9* IndexBufferMvl;
  int IndexBufferFill = 0;
};

}  // namespace DirectX9
}  // namespace Impacto
