#pragma once

#include "../renderer.h"
#include "window.h"

#include "shader.h"

namespace Impacto {
namespace OpenGL {

enum Renderer2DMode {
  R2D_None,
  R2D_Sprite,
  R2D_SpriteInverted,
  R2D_YUVFrame,
  R2D_Masked,
  R2D_CCMessageBox
};

struct VertexBufferSprites {
  glm::vec2 Position;
  glm::vec2 UV;
  glm::vec4 Tint;
  glm::vec2 MaskUV;
};

class Renderer : public IRenderer {
 private:
  void InitImpl();
  void ShutdownImpl();

  void BeginFrameImpl();
  void EndFrameImpl();

  void DrawSpriteImpl(Sprite const& sprite, RectF const& dest,
                      glm::vec4 tint, float angle,
                      bool inverted, bool isScreencap);
  void DrawSpriteImpl(Sprite const& sprite, glm::vec2 topLeft,
                      glm::vec4 tint, glm::vec2 scale, float angle,
                      bool inverted, bool isScreencap);
  void DrawRectImpl(RectF const& dest, glm::vec4 color, float angle);

  void DrawMaskedSpriteImpl(Sprite const& sprite, Sprite const& mask,
                            RectF const& dest, glm::vec4 tint, int alpha,
                            int fadeRange, bool isScreencap,
                            bool isInverted, bool isSameTexture);

  void DrawCCMessageBoxImpl(Sprite const& sprite, Sprite const& mask,
                            glm::vec2 topLeft, glm::vec4 tint, int alpha,
                            int fadeRange, float effectCt, bool isScreencap,
                            glm::vec2 scale);
  void DrawCCMessageBoxImpl(Sprite const& sprite, Sprite const& mask,
                            RectF const& dest, glm::vec4 tint, int alpha,
                            int fadeRange, float effectCt, bool isScreencap);

  void DrawSprite3DRotatedImpl(Sprite const& sprite, RectF const& dest, float depth,
                               glm::vec2 vanishingPoint, bool stayInScreen, glm::quat rot,
                               glm::vec4 tint, bool inverted);
  void DrawSprite3DRotatedImpl(Sprite const& sprite, glm::vec2 topLeft, float depth,
                               glm::vec2 vanishingPoint, bool stayInScreen,
                               glm::quat rot, glm::vec4 tint, glm::vec2 scale, bool inverted);
  void DrawRect3DRotatedImpl(RectF const& dest, float depth, glm::vec2 vanishingPoint,
                             bool stayInScreen, glm::quat rot, glm::vec4 color);

  void DrawProcessedTextImpl(ProcessedTextGlyph* text, int length, Font* font,
                             float opacity, bool outlined, bool smoothstepGlyphOpacity);

  void DrawCharacterMvlImpl(Sprite const& sprite, glm::vec2 topLeft,
                            int verticesCount, float* mvlVertices, int indicesCount,
                            uint16_t* mvlIndices, bool inverted, glm::vec4 tint);

  void DrawVideoTextureImpl(YUVFrame const& tex, RectF const& dest,
                            glm::vec4 tint, float angle, bool alphaVideo);
  void DrawVideoTextureImpl(YUVFrame const& tex, glm::vec2 topLeft,
                            glm::vec4 tint, glm::vec2 scale, float angle,
                            bool alphaVideo);

  void CaptureScreencapImpl(Sprite const& sprite);

  void EnableScissorImpl();
  void SetScissorRectImpl(RectF const& rect);
  void DisableScissorImpl();

  void EnsureSpaceAvailable(int vertices, int vertexSize, int indices);
  void EnsureTextureBound(GLuint texture);
  void EnsureModeSprite(bool inverted);
  void Flush();

  inline void QuadSetUV(RectF const& spriteBounds, float designWidth,
                        float designHeight, uintptr_t uvs, int stride);
  inline void QuadSetUVFlipped(RectF const& spriteBounds,
                               float designWidth, float designHeight,
                               uintptr_t uvs, int stride);
  inline void QuadSetPosition(RectF const& transformedQuad, float angle,
                              uintptr_t positions, int stride);
  inline void QuadSetPosition3DRotated(RectF const& transformedQuad,
                                       float depth,
                                       glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       uintptr_t positions, int stride);

  void DrawProcessedText_BasicFont(ProcessedTextGlyph* text, int length,
                                   BasicFont* font, float opacity,
                                   bool outlined,
                                   bool smoothstepGlyphOpacity);
  void DrawProcessedText_LBFont(ProcessedTextGlyph* text, int length,
                                       LBFont* font, float opacity, bool outlined,
                                       bool smoothstepGlyphOpacity);


  GLWindow* OpenGLWindow;

  GLuint ShaderProgramSprite;
  GLuint ShaderProgramSpriteInverted;
  GLuint ShaderProgramMaskedSprite;
  GLuint ShaderProgramTest;
  GLuint ShaderProgramYUVFrame;
  GLuint ShaderProgramCCMessageBox;

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

} // OpenGL
} // Impacto
