#pragma once

#include <vector>

#include "spritesheet.h"
#include "text/processedtextglyph.h"

namespace Impacto {

enum class RendererOutlineMode : uint8_t { None, BottomRight, Full };

enum class FontType : uint8_t {
  SingleSheet,
  SeparateOutlineSheet,
  LanguageBarrier,
};

enum class OpacityCurve : uint8_t {
  Linear,
  Smoothstep,
  CubedTransparency,
};

class Font {
 public:
  FontType Type;

  virtual size_t GetGlyphCount() const = 0;

  virtual void DrawProcessedText(
      std::span<const ProcessedTextGlyph> text, float opacity,
      float outlineOpacity,
      RendererOutlineMode outlineMode = RendererOutlineMode::None,
      const SpriteSheet* maskedSheet = nullptr,
      glm::mat4 transformation = glm::mat4(1.0f)) = 0;

  void DrawProcessedText(
      std::span<const ProcessedTextGlyph> text, float opacity = 1.0f,
      RendererOutlineMode outlineMode = RendererOutlineMode::None,
      const SpriteSheet* maskedSheet = nullptr,
      glm::mat4 transformation = glm::mat4(1.0f)) {
    DrawProcessedText(text, opacity, opacity, outlineMode, maskedSheet,
                      transformation);
  }

  void DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                         float opacity, float outlineOpacity,
                         RendererOutlineMode outlineMode, glm::vec2 pos,
                         const SpriteSheet* maskedSheet = nullptr) {
    DrawProcessedText(text, opacity, outlineOpacity, outlineMode, maskedSheet,
                      glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f)));
  }

  std::vector<float> AdvanceWidths;

  float BitmapEmWidth;
  float BitmapEmHeight;

  OpacityCurve ForegroundOpacityCurve;
  OpacityCurve OutlineOpacityCurve;

 protected:
  Font(FontType type, float bitmapEmWidth, float bitmapEmHeight,
       OpacityCurve foregroundOpacityCurve, OpacityCurve outlineOpacityCurve)
      : Type(type),
        BitmapEmWidth(bitmapEmWidth),
        BitmapEmHeight(bitmapEmHeight),
        ForegroundOpacityCurve(foregroundOpacityCurve),
        OutlineOpacityCurve(outlineOpacityCurve) {}
};

class SingleSheetFont : public Font {
 public:
  SingleSheetFont(std::optional<float> bitmapEmWidth,
                  std::optional<float> bitmapEmHeight,
                  OpacityCurve foregroundOpacityCurve,
                  OpacityCurve outlineOpacityCurve, SpriteSheet sheet,
                  glm::ivec2 gridSize)
      : Font(FontType::SingleSheet,
             bitmapEmWidth.value_or(sheet.DesignWidth / gridSize.x),
             bitmapEmHeight.value_or(sheet.DesignHeight / gridSize.y),
             foregroundOpacityCurve, outlineOpacityCurve),
        Sheet(sheet),
        GridSize(gridSize),
        CellSize(sheet.GetDimensions() / static_cast<glm::vec2>(gridSize)) {}

  size_t GetGlyphCount() const override {
    return static_cast<size_t>(GridSize.x * GridSize.y);
  }

  void DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                         float opacity, float outlineOpacity,
                         RendererOutlineMode outlineMode,
                         const SpriteSheet* maskedSheet,
                         glm::mat4 transformation) override;

 private:
  Sprite GetGlyph(uint16_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / GridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % GridSize.x);
    const float width = AdvanceWidths[id];

    return Sprite(Sheet, col * CellSize.x + 1.0f, row * CellSize.y + 1.0f,
                  width - 2.0f, BitmapEmHeight - 2.0f);
  }

  SpriteSheet Sheet;
  glm::ivec2 GridSize;
  glm::vec2 CellSize;
};

class SeparateOutlineSheetFont : public Font {
 public:
  SeparateOutlineSheetFont(std::optional<float> bitmapEmWidth,
                           std::optional<float> bitmapEmHeight,
                           OpacityCurve foregroundOpacityCurve,
                           OpacityCurve outlineOpacityCurve,
                           SpriteSheet foregroundSheet,
                           glm::ivec2 foregroundGridSize,
                           SpriteSheet outlineSheet, glm::ivec2 outlineGridSize)
      : SeparateOutlineSheetFont(
            FontType::SeparateOutlineSheet, bitmapEmWidth, bitmapEmHeight,
            foregroundOpacityCurve, outlineOpacityCurve, foregroundSheet,
            foregroundGridSize, outlineSheet, outlineGridSize) {}

  size_t GetGlyphCount() const override {
    return static_cast<size_t>(ForegroundGridSize.x * ForegroundGridSize.y);
  }

  virtual void DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                                 float opacity, float outlineOpacity,
                                 RendererOutlineMode outlineMode,
                                 const SpriteSheet* maskedSheet,
                                 glm::mat4 transformation) override;

 protected:
  SeparateOutlineSheetFont(FontType type, std::optional<float> bitmapEmWidth,
                           std::optional<float> bitmapEmHeight,
                           OpacityCurve foregroundOpacityCurve,
                           OpacityCurve outlineOpacityCurve,
                           SpriteSheet foregroundSheet,
                           glm::ivec2 foregroundGridSize,
                           SpriteSheet outlineSheet, glm::ivec2 outlineGridSize)
      : Font(type,
             bitmapEmWidth.value_or(foregroundSheet.DesignWidth /
                                    foregroundGridSize.x),
             bitmapEmHeight.value_or(foregroundSheet.DesignHeight /
                                     foregroundGridSize.y),
             foregroundOpacityCurve, outlineOpacityCurve),
        ForegroundSheet(foregroundSheet),
        OutlineSheet(outlineSheet),
        ForegroundGridSize(foregroundGridSize),
        OutlineGridSize(outlineGridSize),
        ForegroundCellSize(foregroundSheet.GetDimensions() /
                           static_cast<glm::vec2>(foregroundGridSize)),
        OutlineCellSize(outlineSheet.GetDimensions() /
                        static_cast<glm::vec2>(outlineGridSize)) {}

  SpriteSheet ForegroundSheet;
  SpriteSheet OutlineSheet;

  glm::ivec2 ForegroundGridSize;
  glm::ivec2 OutlineGridSize;

  glm::vec2 ForegroundCellSize;
  glm::vec2 OutlineCellSize;

 private:
  Sprite GetGlyph(uint16_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / ForegroundGridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % ForegroundGridSize.x);
    const float width = AdvanceWidths[id];

    return Sprite(ForegroundSheet, col * ForegroundCellSize.x + 1.0f,
                  row * ForegroundCellSize.y + 1.0f, width - 2.0f,
                  BitmapEmHeight - 2.0f);
  }

  Sprite GetOutlineGlyph(uint16_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / OutlineGridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % OutlineGridSize.x);
    const float width = AdvanceWidths[id];

    return Sprite(OutlineSheet, col * OutlineCellSize.x + 1.0f,
                  row * OutlineCellSize.y + 1.0f, width - 2.0f,
                  BitmapEmHeight - 2.0f);
  }
};

class LanguageBarrierFont : public SeparateOutlineSheetFont {
 public:
  LanguageBarrierFont(std::optional<float> bitmapEmWidth,
                      std::optional<float> bitmapEmHeight,
                      OpacityCurve foregroundOpacityCurve,
                      OpacityCurve outlineOpacityCurve,
                      SpriteSheet foregroundSheet,
                      glm::ivec2 foregroundGridSize, SpriteSheet outlineSheet,
                      glm::ivec2 outlineGridSize, glm::vec2 foregroundOffset,
                      glm::vec2 outlineOffset)
      : SeparateOutlineSheetFont(
            FontType::LanguageBarrier, bitmapEmWidth, bitmapEmHeight,
            foregroundOpacityCurve, outlineOpacityCurve, foregroundSheet,
            foregroundGridSize, outlineSheet, outlineGridSize),
        ForegroundOffset(foregroundOffset),
        OutlineOffset(outlineOffset) {}

  size_t GetGlyphCount() const override {
    return static_cast<size_t>(ForegroundGridSize.x * ForegroundGridSize.y);
  }

  void DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                         float opacity, float outlineOpacity,
                         RendererOutlineMode outlineMode,
                         const SpriteSheet* maskedSheet,
                         glm::mat4 transformation) override;

 private:
  Sprite GetGlyph(uint16_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / ForegroundGridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % ForegroundGridSize.x);

    return Sprite(ForegroundSheet, col * ForegroundCellSize.x,
                  row * ForegroundCellSize.y, ForegroundCellSize.x,
                  ForegroundCellSize.y);
  }

  Sprite GetOutlineGlyph(uint16_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / OutlineGridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % OutlineGridSize.x);

    return Sprite(OutlineSheet, col * OutlineCellSize.x,
                  row * OutlineCellSize.y, OutlineCellSize.x,
                  OutlineCellSize.y);
  }

  glm::vec2 ForegroundOffset;
  glm::vec2 OutlineOffset;
};

}  // namespace Impacto