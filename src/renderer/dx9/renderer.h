#pragma once

#include "../renderer.h"
#include "window.h"
#include "shader.h"
#include "yuvframe.h"

#include <d3d9.h>

namespace Impacto {
namespace DirectX9 {

struct VertexBufferSprites {
  glm::vec2 Position;
  glm::vec4 Tint;
  glm::vec2 UV;
  glm::vec2 MaskUV;
};

class Renderer : public BaseRenderer {
 private:
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
  YUVFrame* CreateYUVFrameImpl(float width, float height) override;

  void DrawSpriteImpl(Sprite const& sprite, RectF const& dest, glm::vec4 tint,
                      float angle, bool inverted, bool isScreencap) override;

  void DrawSpriteImpl(Sprite const& sprite,
                      std::array<glm::vec2, 4> const& dest,
                      const std::array<glm::vec4, 4>& tints, float angle,
                      bool inverted, bool isScreencap) override;

  void DrawSpriteOffsetImpl(Sprite const& sprite, glm::vec2 topLeft,
                            glm::vec2 displayOffset, glm::vec4 tint,
                            glm::vec2 scale, float angle,
                            bool inverted) override;

  void DrawRectImpl(RectF const& dest, glm::vec4 color, float angle) override;

  void DrawMaskedSpriteImpl(Sprite const& sprite, Sprite const& mask,
                            RectF const& dest, glm::vec4 tint, int alpha,
                            int fadeRange, bool isScreencap, bool isInverted,
                            bool isSameTexture) override;

  void DrawCCMessageBoxImpl(Sprite const& sprite, Sprite const& mask,
                            RectF const& dest, glm::vec4 tint, int alpha,
                            int fadeRange, float effectCt,
                            bool isScreencap) override;

  void DrawCHLCCDelusionOverlayImpl(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, int alpha, int fadeRange,
                                    float angle) override;

  void DrawCHLCCMenuBackgroundImpl(const Sprite& sprite, const Sprite& mask,
                                   const RectF& dest, float alpha) override;

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
                            bool inverted, glm::vec4 tint, glm::vec2 scale) override;

  void DrawVideoTextureImpl(YUVFrame* tex, RectF const& dest, glm::vec4 tint,
                            float angle, bool alphaVideo) override;

  void CaptureScreencapImpl(Sprite const& sprite) override;

  void EnableScissorImpl() override;
  void SetScissorRectImpl(RectF const& rect) override;
  void DisableScissorImpl() override;

  void EnsureSpaceAvailable(int vertices, int vertexSize, int indices);
  void EnsureTextureBound(uint32_t texture);
  void Renderer::EnsureShader(Shader* shader, bool flush = true);
  void Flush();

  inline void QuadSetUV(RectF const& spriteBounds, float designWidth,
                        float designHeight, uintptr_t uvs, int stride,
                        float angle = 0.0f);
  inline void QuadSetPositionOffset(RectF const& spriteBounds,
                                    glm::vec2 topLeftPos,
                                    glm::vec2 displayOffset, glm::vec2 scale,
                                    float angle, uintptr_t positions,
                                    int stride);
  inline void QuadSetUVFlipped(RectF const& spriteBounds, float designWidth,
                               float designHeight, uintptr_t uvs, int stride);
  inline void QuadSetPosition(RectF const& transformedQuad, float angle,
                              uintptr_t positions, int stride);
  inline void QuadSetPosition(std::array<glm::vec2, 4> const& destQuad,
                              float angle, uintptr_t positions, int stride);
  inline void QuadSetPosition3DRotated(RectF const& transformedQuad,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       uintptr_t positions, int stride);

  DirectX9Window* DXWindow;

  IDirect3D9* Interface;
  IDirect3DDevice9* Device;

  Shader* CurrentShader;

  Shader* ShaderSprite;
  Shader* ShaderSpriteInverted;
  Shader* ShaderMaskedSprite;
  Shader* ShaderYUVFrame;
  Shader* ShaderCCMessageBox;
  Shader* ShaderCHLCCMenuBackground;

  uint32_t CurrentTexture = 0;
  uint32_t NextTextureId = 1;

  DX9YUVFrame* VideoFrameInternal;

  bool Drawing = false;

  static int const VertexBufferSize = 1024 * 1024;
  static int const IndexBufferCount =
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
