#pragma once

#include "window.h"

#include "3d/scene.h"
#include "../spritesheet.h"
#include "../text.h"
#include "yuvframe.h"
#include <span>

namespace Impacto {

inline GraphicsApi GraphicsApiHint;
inline GraphicsApi ActualGraphicsApi;

enum class RendererOutlineMode { None, BottomRight, Full };

constexpr inline int MaxFramebuffers = 10;

class BaseRenderer {
 public:
  virtual void Init() = 0;
  virtual void Shutdown() = 0;

#ifndef IMPACTO_DISABLE_IMGUI
  virtual void ImGuiBeginFrame() = 0;
#endif

  virtual void BeginFrame() = 0;
  virtual void BeginFrame2D() = 0;
  virtual void EndFrame() = 0;

  virtual uint32_t SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                                 int height) = 0;

  std::vector<uint8_t> GetSpriteSheetImage(SpriteSheet const& sheet) {
    std::vector<uint8_t> result(sheet.DesignWidth * sheet.DesignHeight * 4);
    GetSpriteSheetImage(sheet, result);
    return result;
  };

  virtual int GetSpriteSheetImage(SpriteSheet const& sheet,
                                  std::span<uint8_t> outBuffer) = 0;
  virtual void FreeTexture(uint32_t id) = 0;
  virtual YUVFrame* CreateYUVFrame(float width, float height) = 0;

  virtual void DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                          const std::array<glm::vec4, 4>& tints,
                          bool inverted = false) = 0;
  void DrawSprite(const Sprite& sprite, glm::mat4 transformation,
                  glm::vec4 tint = glm::vec4(1.0f), bool inverted = false);

  // TODO: Remove entirely
  void DrawSprite(Sprite const& sprite, RectF const& dest,
                  glm::vec4 tint = glm::vec4(1.0), float angle = 0.0f,
                  bool inverted = false);
  // TODO: Remove entirely
  void DrawSprite(Sprite const& sprite, glm::vec2 topLeft,
                  glm::vec4 tint = glm::vec4(1.0),
                  glm::vec2 scale = glm::vec2(1.0), float angle = 0.0f,
                  bool inverted = false);

  // TODO: Remove entirely
  void DrawSpriteOffset(Sprite const& sprite, glm::vec2 topLeft,
                        glm::vec2 displayOffset,
                        glm::vec4 tint = glm::vec4(1.0),
                        glm::vec2 scale = glm::vec2(1.0), float angle = 0.0f,
                        bool inverted = false);

  virtual void DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                                const CornersQuad& dest, int alpha,
                                int fadeRange,
                                const std::array<glm::vec4, 4>& tints,
                                bool isInverted = false,
                                bool isSameTexture = false) {}
  void DrawMaskedSprite(const Sprite& sprite, const Sprite& mask, int alpha,
                        int fadeRange, glm::mat4 transformation,
                        glm::vec4 tint = glm::vec4(1.0f),
                        bool isInverted = false, bool isSameTexture = false);

  // TODO: Remove entirely
  void DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, bool isInverted = false,
                        bool isSameTexture = false);

  virtual void DrawVertices(SpriteSheet const& sheet,
                            std::span<const glm::vec2> sheetPositions,
                            std::span<const glm::vec2> displayPositions,
                            int width, int height,
                            glm::vec4 tint = glm::vec4(1.0),
                            bool inverted = false,
                            bool disableBlend = false) = 0;

  void DrawRect(const RectF& dest, glm::vec4 color, float angle = 0.0f);

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        glm::vec2 topLeft, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt,
                        glm::vec2 scale = glm::vec2(1.0));
  virtual void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                RectF const& dest, glm::vec4 tint, int alpha,
                                int fadeRange, float effectCt) = 0;

  virtual void DrawCHLCCMenuBackground(const Sprite& sprite, const Sprite& mask,
                                       const RectF& dest, float alpha) = 0;

  virtual void DrawMaskedSpriteOverlay(Sprite const& sprite, Sprite const& mask,
                                       RectF const& dest, glm::vec4 tint,
                                       int alpha, int fadeRange,
                                       bool isInverted = false,
                                       float angle = 0.0f,
                                       bool useMaskAlpha = true) = 0;

  // TODO: Remove entirely
  void DrawSprite3DRotated(Sprite const& sprite, RectF const& dest, float depth,
                           glm::vec2 vanishingPoint, bool stayInScreen,
                           glm::quat rot, glm::vec4 tint = glm::vec4(1.0f),
                           bool inverted = false);

  // TODO: Remove entirely
  void DrawSprite3DRotated(Sprite const& sprite, glm::vec2 topLeft, float depth,
                           glm::vec2 vanishingPoint, bool stayInScreen,
                           glm::quat rot, glm::vec4 tint = glm::vec4(1.0f),
                           glm::vec2 scale = glm::vec2(1.0f),
                           bool inverted = false);

  // TODO: Remove entirely
  virtual void DrawRect3DRotated(RectF const& dest, float depth,
                                 glm::vec2 vanishingPoint, bool stayInScreen,
                                 glm::quat rot, glm::vec4 color) = 0;

  void DrawProcessedText_BasicFont(std::span<const ProcessedTextGlyph> text,
                                   BasicFont* font, float opacity,
                                   RendererOutlineMode outlineMode,
                                   bool smoothstepGlyphOpacity,
                                   float outlineOpacity,
                                   SpriteSheet* maskedSheet);

  void DrawProcessedText_LBFont(std::span<const ProcessedTextGlyph> text,
                                LBFont* font, float opacity,
                                RendererOutlineMode outlineMode,
                                bool smoothstepGlyphOpacity,
                                float outlineOpacity, SpriteSheet* maskedSheet);
  void DrawProcessedText(
      std::span<const ProcessedTextGlyph> text, Font* font,
      float opacity = 1.0f,
      RendererOutlineMode outlineMode = RendererOutlineMode::None,
      bool smoothstepGlyphOpacity = true, SpriteSheet* maskedSheet = 0);

  void DrawProcessedText(
      std::span<const ProcessedTextGlyph> text, Font* font, float opacity,
      float outlineOpacity,
      RendererOutlineMode outlineMode = RendererOutlineMode::None,
      bool smoothstepGlyphOpacity = true, SpriteSheet* maskedSheet = 0);

  virtual void DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                                int verticesCount, float* mvlVertices,
                                int indicesCount, uint16_t* mvlIndices,
                                bool inverted = false,
                                glm::vec4 tint = glm::vec4(1.0),
                                glm::vec2 scale = glm::vec2(1.0f)) = 0;

  virtual void DrawVideoTexture(YUVFrame* tex, RectF const& dest,
                                glm::vec4 tint = glm::vec4(1.0),
                                float angle = 0.0f,
                                bool alphaVideo = false) = 0;
  void DrawVideoTexture(YUVFrame* tex, glm::vec2 topLeft,
                        glm::vec4 tint = glm::vec4(1.0),
                        glm::vec2 scale = glm::vec2(1.0), float angle = 0.0f,
                        bool alphaVideo = false);

  virtual void CaptureScreencap(Sprite& sprite) = 0;

  virtual void SetFramebuffer(size_t buffer) = 0;
  virtual int GetFramebufferTexture(size_t buffer) = 0;

  virtual void EnableScissor() = 0;
  virtual void SetScissorRect(RectF const& rect) = 0;
  virtual void DisableScissor() = 0;

  bool IsInit = false;

  IScene3D* Scene = 0;

 protected:
  virtual void Flush() = 0;

  struct VertexBufferSprites {
    glm::vec2 Position;
    glm::vec2 UV;
    glm::vec4 Tint;
    glm::vec2 MaskUV;
  };

  static void QuadSetUV(const RectF spriteBounds, float designWidth,
                        float designHeight, glm::vec2* uvs, size_t stride,
                        float angle = 0.0f);

  static void QuadSetUVFlipped(const RectF spriteBounds, float designWidth,
                               float designHeight, glm::vec2* uvs,
                               size_t stride) {
    QuadSetUV({spriteBounds.X, spriteBounds.Y + spriteBounds.Height,
               spriteBounds.Width, -spriteBounds.Height},
              designWidth, designHeight, uvs, stride);
  }

  static void QuadSetPosition(RectF const& transformedQuad, float angle,
                              uintptr_t positions, int stride);
  static void QuadSetPosition(CornersQuad destQuad, float angle,
                              uintptr_t positions, int stride);

  static void QuadSetPositionOffset(RectF const& spriteBounds,
                                    glm::vec2 displayXY,
                                    glm::vec2 displayOffset, glm::vec2 scale,
                                    float angle, uintptr_t positions,
                                    int stride, bool toNDC = true);
  static void QuadSetPosition3DRotated(RectF const& transformedQuad,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       uintptr_t positions, int stride);

  Sprite RectSprite;
};

inline BaseRenderer* Renderer;
inline BaseWindow* Window;

void InitRenderer();

}  // namespace Impacto
