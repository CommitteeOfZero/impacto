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

  void DrawSprite(Sprite const& sprite, CornersQuad const& dest,
                  const std::array<glm::vec4, 4>& tints, float angle = 0.0f,
                  bool inverted = false) override;

  void DrawSpriteOffset(Sprite const& sprite, glm::vec2 topLeft,
                        glm::vec2 displayOffset,
                        glm::vec4 tint = glm::vec4(1.0),
                        glm::vec2 scale = glm::vec2(1.0), float angle = 0.0f,
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

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt) override;

  void DrawMaskedSpriteOverlay(Sprite const& sprite, Sprite const& mask,
                               RectF const& dest, glm::vec4 tint, int alpha,
                               int fadeRange, bool isInverted, float angle,
                               bool useMaskAlpha) override;

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
  void EnsureSpaceAvailable(int vertices, int vertexSize, int indices);
  void EnsureTextureBound(uint32_t texture);
  void EnsureShader(Shader* shader, bool flush = true);
  void Flush();

  void QuadSetUV(RectF const& spriteBounds, float designWidth,
                 float designHeight, uintptr_t uvs, int stride,
                 float angle = 0.0f);
  void QuadSetPositionOffset(RectF const& spriteBounds, glm::vec2 topLeftPos,
                             glm::vec2 displayOffset, glm::vec2 scale,
                             float angle, uintptr_t positions, int stride);
  void QuadSetUVFlipped(RectF const& spriteBounds, float designWidth,
                        float designHeight, uintptr_t uvs, int stride);
  void QuadSetPosition(RectF const& transformedQuad, float angle,
                       uintptr_t positions, int stride);
  void QuadSetPosition(CornersQuad destQuad, float angle, uintptr_t positions,
                       int stride);
  void QuadSetPosition3DRotated(RectF const& transformedQuad, float depth,
                                glm::vec2 vanishingPoint, bool stayInScreen,
                                glm::quat rot, uintptr_t positions, int stride);

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

  Sprite RectSprite;
};

}  // namespace DirectX9
}  // namespace Impacto
