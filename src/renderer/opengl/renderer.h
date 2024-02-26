#pragma once

#include "../renderer.h"
#include "window.h"

#include <map>

#include "shader.h"

namespace Impacto {
namespace OpenGL {

static int const NkMaxVertexMemory = 256 * 1024;
static int const NkMaxElementMemory = 128 * 1024;

enum Renderer2DMode {
  R2D_None,
  R2D_Sprite,
  R2D_SpriteInverted,
  R2D_YUVFrame,
  R2D_Masked,
  R2D_CCMessageBox,
  R2D_CHLCCMenuBackground
};

struct VertexBufferSprites {
  glm::vec2 Position;
  glm::vec2 UV;
  glm::vec4 Tint;
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
  YUVFrame* CreateYUVFrameImpl(int width, int height) override;

  void DrawSpriteImpl(Sprite const& sprite, RectF const& dest, glm::vec4 tint,
                      float angle, bool inverted, bool isScreencap) override;

  void DrawSpriteImpl(Sprite const& sprite,
                      std::array<glm::vec2, 4> const& dest,
                      const std::array<glm::vec4, 4>& tints, float angle,
                      bool inverted, bool isScreencap) override;

  void DrawSpriteOffsetImpl(Sprite const& sprite, glm::vec2 topLeft,
                            glm::vec2 displayOffset, glm::vec4 tint,
                            glm::vec2 scale, float angle, bool inverted);

  void DrawRectImpl(RectF const& dest, glm::vec4 color, float angle) override;

  void DrawMaskedSpriteImpl(Sprite const& sprite, Sprite const& mask,
                            RectF const& dest, glm::vec4 tint, int alpha,
                            int fadeRange, bool isScreencap, bool isInverted,
                            bool isSameTexture) override;

  void DrawCCMessageBoxImpl(Sprite const& sprite, Sprite const& mask,
                            RectF const& dest, glm::vec4 tint, int alpha,
                            int fadeRange, float effectCt,
                            bool isScreencap) override;

  void DrawCHLCCMenuBackgroundImpl(const Sprite& sprite, const Sprite& mask,
                                   const RectF& dest, float alpha) override;

  void DrawCHLCCDelusionOverlayImpl(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, int alpha, int fadeRange,
                                    float angle) override;

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

  void EnsureSpaceAvailable(int vertices, int vertexSize, int indices);
  void EnsureTextureBound(GLuint texture);
  void EnsureModeSprite(bool inverted);
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

  GLWindow* OpenGLWindow;

  GLuint ShaderProgramSprite;
  GLuint ShaderProgramSpriteInverted;
  GLuint ShaderProgramMaskedSprite;
  GLuint ShaderProgramYUVFrame;
  GLuint ShaderProgramCCMessageBox;
  GLuint ShaderProgramCHLCCMenuBackground;

  GLuint YUVFrameCbLocation;
  GLuint YUVFrameCrLocation;
  GLuint YUVFrameIsAlphaLocation;
  GLuint MaskedIsInvertedLocation;
  GLuint MaskedIsSameTextureLocation;

  GLuint VBO;
  GLuint IBO;
  GLuint VAOSprites;

  GLuint Sampler;

  bool Drawing = false;

  static int const VertexBufferSize = 1024 * 1024;
  static int const IndexBufferCount =
      VertexBufferSize / (4 * sizeof(VertexBufferSprites)) * 6;

  GLuint CurrentTexture = 0;
  Renderer2DMode CurrentMode = R2D_None;
  uint8_t VertexBuffer[VertexBufferSize];
  int VertexBufferFill = 0;
  uint16_t IndexBuffer[IndexBufferCount];
  int IndexBufferFill = 0;

  Sprite RectSprite;

  // ShaderCompiler compiler
  ShaderCompiler* Shaders;
};

}  // namespace OpenGL
}  // namespace Impacto
