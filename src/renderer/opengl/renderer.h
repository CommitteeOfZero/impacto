#pragma once

#include "../renderer.h"
#include "window.h"

#include <map>

#include "shader.h"

namespace Impacto {
namespace OpenGL {

int constexpr NkMaxVertexMemory = 256 * 1024;
int constexpr NkMaxElementMemory = 128 * 1024;

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
  void FreeTexture(uint32_t id) override;
  YUVFrame* CreateYUVFrame(float width, float height) override;

  void DrawSprite(Sprite const& sprite, RectF const& dest,
                  glm::vec4 tint = glm::vec4(1.0), float angle = 0.0f,
                  bool inverted = false, bool isScreencap = false) override;

  void DrawSprite(Sprite const& sprite, std::array<glm::vec2, 4> const& dest,
                  const std::array<glm::vec4, 4>& tints, float angle = 0.0f,
                  bool inverted = false, bool isScreencap = false) override;

  void DrawSpriteOffset(Sprite const& sprite, glm::vec2 topLeft,
                        glm::vec2 displayOffset,
                        glm::vec4 tint = glm::vec4(1.0),
                        glm::vec2 scale = glm::vec2(1.0), float angle = 0.0f,
                        bool inverted = false) override;

  void DrawRect(RectF const& dest, glm::vec4 color,
                float angle = 0.0f) override;

  void DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, bool isScreencap = false,
                        bool isInverted = false,
                        bool isSameTexture = false) override;

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt,
                        bool isScreencap = false) override;

  void DrawCHLCCDelusionOverlay(Sprite const& sprite, Sprite const& mask,
                                RectF const& dest, int alpha, int fadeRange,
                                float angle) override;

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

  void CaptureScreencap(Sprite const& sprite) override;

  void EnableScissor() override;
  void SetScissorRect(RectF const& rect) override;
  void DisableScissor() override;

 private:
  void EnsureSpaceAvailable(int vertices, int vertexSize, int indices);
  void EnsureTextureBound(GLuint texture);
  void EnsureModeSprite(bool inverted);
  void Flush();

  void QuadSetUV(RectF const& spriteBounds, float designWidth,
                 float designHeight, uintptr_t uvs, int stride,
                 float angle = 0.0f);
  void QuadSetPositionOffset(RectF const& spriteBounds, glm::vec2 displayXY,
                             glm::vec2 displayOffset, glm::vec2 scale,
                             float angle, uintptr_t positions, int stride);
  void QuadSetUVFlipped(RectF const& spriteBounds, float designWidth,
                        float designHeight, uintptr_t uvs, int stride);
  void QuadSetPosition(RectF const& transformedQuad, float angle,
                       uintptr_t positions, int stride);
  void QuadSetPosition(std::array<glm::vec2, 4> const& destQuad, float angle,
                       uintptr_t positions, int stride);
  void QuadSetPosition3DRotated(RectF const& transformedQuad, float depth,
                                glm::vec2 vanishingPoint, bool stayInScreen,
                                glm::quat rot, uintptr_t positions, int stride);

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

  static int constexpr VertexBufferSize = 1024 * 1024;
  static int constexpr IndexBufferCount =
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
