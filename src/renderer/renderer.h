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
                          std::span<const glm::vec4, 4> tints,
                          bool inverted = false) = 0;
  void DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                  glm::vec4 tint = glm::vec4(1.0f), bool inverted = false) {
    DrawSprite(sprite, dest, std::array<glm::vec4, 4>{tint, tint, tint, tint},
               inverted);
  }
  void DrawSprite(const Sprite& sprite, glm::mat4 transformation,
                  glm::vec4 tint = glm::vec4(1.0f), bool inverted = false);
  void DrawSprite(const Sprite& sprite, glm::vec2 topLeft,
                  glm::vec4 tint = glm::vec4(1.0f), bool inverted = false);

  // TODO: Remove entirely
  void DrawSprite(Sprite const& sprite, glm::vec2 topLeft, glm::vec4 tint,
                  glm::vec2 scale, float angle = 0.0f, bool inverted = false);

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

  virtual void DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                                const CornersQuad& dest, int alpha,
                                int fadeRange,
                                std::span<const glm::vec4, 4> tints,
                                bool isInverted = false,
                                bool isSameTexture = false) = 0;
  void DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                        const CornersQuad& dest, int alpha, int fadeRange,
                        glm::vec4 tint = glm::vec4(1.0f),
                        bool isInverted = false, bool isSameTexture = false) {
    DrawMaskedSprite(sprite, mask, dest, alpha, fadeRange,
                     std::array<glm::vec4, 4>{tint, tint, tint, tint},
                     isInverted, isSameTexture);
  }
  void DrawMaskedSprite(const Sprite& sprite, const Sprite& mask, int alpha,
                        int fadeRange, glm::mat4 transformation,
                        glm::vec4 tint = glm::vec4(1.0f),
                        bool isInverted = false, bool isSameTexture = false);
  void DrawMaskedSprite(const Sprite& sprite, const Sprite& mask, int alpha,
                        int fadeRange, glm::vec2 topLeft,
                        glm::vec4 tint = glm::vec4(1.0f),
                        bool isInverted = false, bool isSameTexture = false);

  virtual void DrawMaskedSpriteOverlay(const Sprite& sprite, const Sprite& mask,
                                       const CornersQuad& dest, int alpha,
                                       int fadeRange,
                                       std::span<const glm::vec4, 4> tints,
                                       bool isInverted = false,
                                       bool useMaskAlpha = true) = 0;
  void DrawMaskedSpriteOverlay(const Sprite& sprite, const Sprite& mask,
                               const CornersQuad& dest, int alpha,
                               int fadeRange, glm::vec4 tint = glm::vec4(1.0f),
                               bool isInverted = false,
                               bool useMaskAlpha = true) {
    DrawMaskedSpriteOverlay(sprite, mask, dest, alpha, fadeRange,
                            std::array<glm::vec4, 4>{tint, tint, tint, tint},
                            isInverted, useMaskAlpha);
  }
  void DrawMaskedSpriteOverlay(const Sprite& sprite, const Sprite& mask,
                               int alpha, int fadeRange,
                               glm::mat4 transformation,
                               glm::vec4 tint = glm::vec4(1.0f),
                               bool isInverted = false,
                               bool useMaskAlpha = true);
  void DrawMaskedSpriteOverlay(const Sprite& sprite, const Sprite& mask,
                               int alpha, int fadeRange, glm::vec2 topLeft,
                               glm::vec4 tint = glm::vec4(1.0f),
                               bool isInverted = false,
                               bool useMaskAlpha = true);

  virtual void DrawVertices(SpriteSheet const& sheet,
                            std::span<const glm::vec2> sheetPositions,
                            std::span<const glm::vec2> displayPositions,
                            int width, int height,
                            glm::vec4 tint = glm::vec4(1.0),
                            bool inverted = false,
                            bool disableBlend = false) = 0;

  void DrawQuad(const CornersQuad& dest, glm::vec4 color);

  // TODO: Remove entirely
  void DrawRect3DRotated(const RectF& dest, float depth,
                         glm::vec2 vanishingPoint, bool stayInScreen,
                         glm::quat rot, glm::vec4 color);

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        glm::vec2 topLeft, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt,
                        glm::vec2 scale = glm::vec2(1.0));
  virtual void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                RectF const& dest, glm::vec4 tint, int alpha,
                                int fadeRange, float effectCt) = 0;

  virtual void DrawCHLCCMenuBackground(const Sprite& sprite, const Sprite& mask,
                                       const RectF& dest, float alpha) = 0;

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

  virtual void DrawCharacterMvl(const Sprite& sprite,
                                std::span<const float> vertices,
                                std::span<const uint16_t> indices,
                                glm::vec4 tint = glm::vec4(1.0f),
                                bool inverted = false) = 0;
  void DrawCharacterMvl(const Sprite& sprite, std::span<float> vertices,
                        std::span<const uint16_t> indices,
                        glm::mat4 transformation,
                        glm::vec4 tint = glm::vec4(1.0f),
                        bool inverted = false);
  void DrawCharacterMvl(const Sprite& sprite, std::span<float> vertices,
                        std::span<const uint16_t> indices, glm::vec2 offset,
                        glm::vec4 tint = glm::vec4(1.0f),
                        bool inverted = false);

  // TODO: Remove entirely
  void DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                        int verticesCount, float* mvlVertices, int indicesCount,
                        uint16_t* mvlIndices, bool inverted = false,
                        glm::vec4 tint = glm::vec4(1.0),
                        glm::vec2 scale = glm::vec2(1.0f));

  virtual void DrawVideoTexture(const YUVFrame& frame, const RectF& dest,
                                glm::vec4 tint, bool alphaVideo = false) = 0;

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

  static void QuadSetUV(CornersQuad spriteBounds, glm::vec2 designDimensions,
                        glm::vec2* uvs, size_t stride);

  static void QuadSetUVFlipped(CornersQuad spriteBounds,
                               glm::vec2 designDimensions, glm::vec2* uvs,
                               size_t stride) {
    QuadSetUV({spriteBounds.BottomLeft, spriteBounds.TopLeft,
               spriteBounds.BottomRight, spriteBounds.TopRight},
              designDimensions, uvs, stride);
  }

  void QuadSetPosition(CornersQuad destQuad, glm::vec2* positions, int stride);

  virtual glm::vec2 DesignToNDC(glm::vec2 designCoord) const = 0;

  Sprite RectSprite;
};

inline BaseRenderer* Renderer;
inline BaseWindow* Window;

void InitRenderer();

}  // namespace Impacto
