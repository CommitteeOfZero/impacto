#pragma once

#include "window.h"

#include "3d/scene.h"
#include "../spritesheet.h"
#include "../text.h"
#include "yuvframe.h"

namespace Impacto {

extern GraphicsApi GraphicsApiHint;
extern GraphicsApi ActualGraphicsApi;

class BaseRenderer {
 public:
  void Init();
  void Shutdown();

  void BeginFrame();
  void EndFrame();

  uint32_t SubmitTexture(TexFmt format, uint8_t* buffer, int width, int height);
  void FreeTexture(uint32_t id);
  YUVFrame* CreateYUVFrame(int width, int height);

  void DrawSprite(Sprite const& sprite, RectF const& dest,
                  glm::vec4 tint = glm::vec4(1.0), float angle = 0.0f,
                  bool inverted = false, bool isScreencap = false);
  void DrawSprite(Sprite const& sprite, glm::vec2 topLeft,
                  glm::vec4 tint = glm::vec4(1.0),
                  glm::vec2 scale = glm::vec2(1.0), float angle = 0.0f,
                  bool inverted = false, bool isScreencap = false);

  void DrawRect(RectF const& dest, glm::vec4 color, float angle = 0.0f);

  void DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, bool isScreencap = false,
                        bool isInverted = false, bool isSameTexture = false);

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        glm::vec2 topLeft, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt, bool isScreencap = false,
                        glm::vec2 scale = glm::vec2(1.0));
  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt,
                        bool isScreencap = false);

  void DrawSprite3DRotated(Sprite const& sprite, RectF const& dest, float depth,
                           glm::vec2 vanishingPoint, bool stayInScreen,
                           glm::quat rot, glm::vec4 tint = glm::vec4(1.0f),
                           bool inverted = false);
  void DrawSprite3DRotated(Sprite const& sprite, glm::vec2 topLeft, float depth,
                           glm::vec2 vanishingPoint, bool stayInScreen,
                           glm::quat rot, glm::vec4 tint = glm::vec4(1.0f),
                           glm::vec2 scale = glm::vec2(1.0f),
                           bool inverted = false);
  void DrawRect3DRotated(RectF const& dest, float depth,
                         glm::vec2 vanishingPoint, bool stayInScreen,
                         glm::quat rot, glm::vec4 color);

  void DrawProcessedText(ProcessedTextGlyph* text, int length, Font* font,
                         float opacity = 1.0f, bool outlined = false,
                         bool smoothstepGlyphOpacity = true);

  void DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                        int verticesCount, float* mvlVertices, int indicesCount,
                        uint16_t* mvlIndices, bool inverted = false,
                        glm::vec4 tint = glm::vec4(1.0));

  void DrawVideoTexture(YUVFrame* tex, RectF const& dest,
                        glm::vec4 tint = glm::vec4(1.0), float angle = 0.0f,
                        bool alphaVideo = false);
  void DrawVideoTexture(YUVFrame* tex, glm::vec2 topLeft,
                        glm::vec4 tint = glm::vec4(1.0),
                        glm::vec2 scale = glm::vec2(1.0), float angle = 0.0f,
                        bool alphaVideo = false);

  void CaptureScreencap(Sprite const& sprite);

  void EnableScissor();
  void SetScissorRect(RectF const& rect);
  void DisableScissor();

  bool IsInit = false;

  IScene3D* Scene = 0;

 private:
  virtual void InitImpl() = 0;
  virtual void ShutdownImpl() = 0;

  virtual void BeginFrameImpl() = 0;
  virtual void EndFrameImpl() = 0;

  virtual uint32_t SubmitTextureImpl(TexFmt format, uint8_t* buffer, int width,
                                     int height) = 0;
  virtual void FreeTextureImpl(uint32_t id) = 0;

  virtual YUVFrame* CreateYUVFrameImpl(int width, int height) = 0;

  virtual void DrawSpriteImpl(Sprite const& sprite, RectF const& dest,
                              glm::vec4 tint, float angle, bool inverted,
                              bool isScreencap) = 0;
  virtual void DrawRectImpl(RectF const& dest, glm::vec4 color,
                            float angle) = 0;

  virtual void DrawMaskedSpriteImpl(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, glm::vec4 tint,
                                    int alpha, int fadeRange, bool isScreencap,
                                    bool isInverted, bool isSameTexture) = 0;

  virtual void DrawCCMessageBoxImpl(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    bool isScreencap) = 0;

  virtual void DrawSprite3DRotatedImpl(Sprite const& sprite, RectF const& dest,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       glm::vec4 tint, bool inverted) = 0;
  virtual void DrawRect3DRotatedImpl(RectF const& dest, float depth,
                                     glm::vec2 vanishingPoint,
                                     bool stayInScreen, glm::quat rot,
                                     glm::vec4 color) = 0;

  virtual void DrawProcessedTextImpl(ProcessedTextGlyph* text, int length,
                                     Font* font, float opacity, bool outlined,
                                     bool smoothstepGlyphOpacity) = 0;

  virtual void DrawCharacterMvlImpl(Sprite const& sprite, glm::vec2 topLeft,
                                    int verticesCount, float* mvlVertices,
                                    int indicesCount, uint16_t* mvlIndices,
                                    bool inverted, glm::vec4 tint) = 0;

  virtual void DrawVideoTextureImpl(YUVFrame* tex, RectF const& dest,
                                    glm::vec4 tint, float angle,
                                    bool alphaVideo) = 0;

  virtual void CaptureScreencapImpl(Sprite const& sprite) = 0;

  virtual void EnableScissorImpl() = 0;
  virtual void SetScissorRectImpl(RectF const& rect) = 0;
  virtual void DisableScissorImpl() = 0;
};

extern BaseRenderer* Renderer;
extern BaseWindow* Window;

void InitRenderer();

}  // namespace Impacto
