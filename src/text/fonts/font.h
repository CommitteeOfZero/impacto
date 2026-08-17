#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "../../spritesheet.h"
#include "../processedtextglyph.h"

namespace Impacto::Fonts {

enum class RendererOutlineMode : uint8_t { None, BottomRight, Full };

enum class FontType : uint8_t {
  SingleSheet,
  SeparateOutlineSheet,
  LanguageBarrier,
  EdgeDetectedSingleSheet,
  External,
};

enum class OpacityCurve : uint8_t {
  Linear,
  Smoothstep,
  CubedTransparency,
};

class Font {
 public:
  FontType Type;

  virtual ~Font() = default;

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

  virtual float GetAdvanceWidth(uint32_t glyphId) const = 0;

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

  static float ApplyOpacityCurve(float opacity, OpacityCurve curve);
  static std::vector<size_t> GetVisibleGlyphIds(
      std::span<const ProcessedTextGlyph> text);
};

class SingleSheetFont : public Font {
 public:
  SingleSheetFont(float bitmapEmWidth, float bitmapEmHeight,
                  OpacityCurve foregroundOpacityCurve,
                  OpacityCurve outlineOpacityCurve, SpriteSheet sheet,
                  glm::ivec2 gridSize, std::vector<float>&& advanceWidths)
      : SingleSheetFont(FontType::SingleSheet, bitmapEmWidth, bitmapEmHeight,
                        foregroundOpacityCurve, outlineOpacityCurve, sheet,
                        gridSize, std::move(advanceWidths)) {}

  virtual void DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                                 float opacity, float outlineOpacity,
                                 RendererOutlineMode outlineMode,
                                 const SpriteSheet* maskedSheet,
                                 glm::mat4 transformation) override;

  virtual float GetAdvanceWidth(uint32_t glyphId) const override {
    return AdvanceWidths[glyphId];
  }

 protected:
  SpriteSheet Sheet;
  glm::ivec2 GridSize;
  glm::vec2 CellSize;

  std::vector<float> AdvanceWidths;

  SingleSheetFont(FontType fontType, float bitmapEmWidth, float bitmapEmHeight,
                  OpacityCurve foregroundOpacityCurve,
                  OpacityCurve outlineOpacityCurve, SpriteSheet sheet,
                  glm::ivec2 gridSize, std::vector<float>&& advanceWidths)
      : Font(fontType, bitmapEmWidth, bitmapEmHeight, foregroundOpacityCurve,
             outlineOpacityCurve),
        Sheet(sheet),
        GridSize(gridSize),
        CellSize(sheet.GetDimensions() / static_cast<glm::vec2>(gridSize)),
        AdvanceWidths(std::move(advanceWidths)) {}

 private:
  Sprite GetGlyph(uint32_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / GridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % GridSize.x);
    const float width = AdvanceWidths[id];

    return Sprite(Sheet, col * CellSize.x + 1.0f, row * CellSize.y + 1.0f,
                  width - 2.0f, BitmapEmHeight - 2.0f);
  }
};

class SeparateOutlineSheetFont : public Font {
 public:
  SeparateOutlineSheetFont(float bitmapEmWidth, float bitmapEmHeight,
                           OpacityCurve foregroundOpacityCurve,
                           OpacityCurve outlineOpacityCurve,
                           SpriteSheet foregroundSheet,
                           glm::ivec2 foregroundGridSize,
                           SpriteSheet outlineSheet, glm::ivec2 outlineGridSize,
                           std::vector<float>&& advanceWidths)
      : SeparateOutlineSheetFont(FontType::SeparateOutlineSheet, bitmapEmWidth,
                                 bitmapEmHeight, foregroundOpacityCurve,
                                 outlineOpacityCurve, foregroundSheet,
                                 foregroundGridSize, outlineSheet,
                                 outlineGridSize, std::move(advanceWidths)) {}

  virtual void DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                                 float opacity, float outlineOpacity,
                                 RendererOutlineMode outlineMode,
                                 const SpriteSheet* maskedSheet,
                                 glm::mat4 transformation) override;

  virtual float GetAdvanceWidth(uint32_t glyphId) const override {
    return AdvanceWidths[glyphId];
  }

 protected:
  SeparateOutlineSheetFont(FontType type, float bitmapEmWidth,
                           float bitmapEmHeight,
                           OpacityCurve foregroundOpacityCurve,
                           OpacityCurve outlineOpacityCurve,
                           SpriteSheet foregroundSheet,
                           glm::ivec2 foregroundGridSize,
                           SpriteSheet outlineSheet, glm::ivec2 outlineGridSize,
                           std::vector<float>&& advanceWidths)
      : Font(type, bitmapEmWidth, bitmapEmHeight, foregroundOpacityCurve,
             outlineOpacityCurve),
        ForegroundSheet(foregroundSheet),
        OutlineSheet(outlineSheet),
        ForegroundGridSize(foregroundGridSize),
        OutlineGridSize(outlineGridSize),
        ForegroundCellSize(foregroundSheet.GetDimensions() /
                           static_cast<glm::vec2>(foregroundGridSize)),
        OutlineCellSize(outlineSheet.GetDimensions() /
                        static_cast<glm::vec2>(outlineGridSize)),
        AdvanceWidths(std::move(advanceWidths)) {}

  SpriteSheet ForegroundSheet;
  SpriteSheet OutlineSheet;

  glm::ivec2 ForegroundGridSize;
  glm::ivec2 OutlineGridSize;

  glm::vec2 ForegroundCellSize;
  glm::vec2 OutlineCellSize;

  std::vector<float> AdvanceWidths;

 private:
  Sprite GetGlyph(uint32_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / ForegroundGridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % ForegroundGridSize.x);
    const float width = AdvanceWidths[id];

    return Sprite(ForegroundSheet, col * ForegroundCellSize.x + 1.0f,
                  row * ForegroundCellSize.y + 1.0f, width - 2.0f,
                  BitmapEmHeight - 2.0f);
  }

  Sprite GetOutlineGlyph(uint32_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / OutlineGridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % OutlineGridSize.x);
    const float width = AdvanceWidths[id];

    return Sprite(OutlineSheet, col * OutlineCellSize.x + 1.0f,
                  row * OutlineCellSize.y + 1.0f, width - 2.0f,
                  BitmapEmHeight - 2.0f);
  }
};

class LanguageBarrierFont final : public SeparateOutlineSheetFont {
 public:
  LanguageBarrierFont(float bitmapEmWidth, float bitmapEmHeight,
                      OpacityCurve foregroundOpacityCurve,
                      OpacityCurve outlineOpacityCurve,
                      SpriteSheet foregroundSheet,
                      glm::ivec2 foregroundGridSize, SpriteSheet outlineSheet,
                      glm::ivec2 outlineGridSize,
                      std::vector<float>&& advanceWidths,
                      glm::vec2 foregroundOffset, glm::vec2 outlineOffset)
      : SeparateOutlineSheetFont(FontType::LanguageBarrier, bitmapEmWidth,
                                 bitmapEmHeight, foregroundOpacityCurve,
                                 outlineOpacityCurve, foregroundSheet,
                                 foregroundGridSize, outlineSheet,
                                 outlineGridSize, std::move(advanceWidths)),
        ForegroundOffset(foregroundOffset),
        OutlineOffset(outlineOffset) {}

  void DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                         float opacity, float outlineOpacity,
                         RendererOutlineMode outlineMode,
                         const SpriteSheet* maskedSheet,
                         glm::mat4 transformation) override;

 private:
  Sprite GetGlyph(uint32_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / ForegroundGridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % ForegroundGridSize.x);

    return Sprite(ForegroundSheet, col * ForegroundCellSize.x,
                  row * ForegroundCellSize.y, ForegroundCellSize.x,
                  ForegroundCellSize.y);
  }

  Sprite GetOutlineGlyph(uint32_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / OutlineGridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % OutlineGridSize.x);

    return Sprite(OutlineSheet, col * OutlineCellSize.x,
                  row * OutlineCellSize.y, OutlineCellSize.x,
                  OutlineCellSize.y);
  }

  glm::vec2 ForegroundOffset;
  glm::vec2 OutlineOffset;
};

class EdgeDetectedSingleSheetFont final : public SingleSheetFont {
 public:
  EdgeDetectedSingleSheetFont(float bitmapEmWidth, float bitmapEmHeight,
                              OpacityCurve opacityCurve, SpriteSheet sheet,
                              glm::ivec2 gridSize,
                              std::vector<float>&& advanceWidths)
      : SingleSheetFont(FontType::EdgeDetectedSingleSheet, bitmapEmWidth,
                        bitmapEmHeight, opacityCurve, opacityCurve, sheet,
                        gridSize, std::move(advanceWidths)) {}

  void DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                         float opacity, float outlineOpacity,
                         RendererOutlineMode outlineMode,
                         const SpriteSheet* maskedSheet,
                         glm::mat4 transformation) override;

 private:
  Sprite GetGlyph(uint32_t id) const {
    const uint8_t row = static_cast<uint8_t>(id / GridSize.x);
    const uint8_t col = static_cast<uint8_t>(id % GridSize.x);
    const float width = AdvanceWidths[id];

    return Sprite(Sheet, col * CellSize.x, row * CellSize.y, width,
                  BitmapEmHeight);
  }
};

struct ExternalFontShapedGlyph {
  uint32_t GlyphIndex;
  glm::vec2 Offset;
  glm::vec2 Advance;
};

class ExternalFont final : public Font {
 public:
  ExternalFont(std::string const& path, std::string const& logContext);
  ~ExternalFont() override;

  ExternalFont(ExternalFont const&) = delete;
  ExternalFont& operator=(ExternalFont const&) = delete;

  bool IsLoaded() const;

  std::vector<ExternalFontShapedGlyph> ShapeLine(std::string_view text,
                                                 float fontSize, float& width);

  using Font::DrawProcessedText;
  void DrawProcessedText(
      std::span<const ProcessedTextGlyph> text, float opacity,
      float outlineOpacity,
      RendererOutlineMode outlineMode = RendererOutlineMode::None,
      const SpriteSheet* maskedSheet = nullptr,
      glm::mat4 transformation = glm::mat4(1.0f)) override;

  float GetAdvanceWidth(uint32_t glyphId) const override {
    assert(false && "TODO: Implement");
    return BitmapEmWidth;
  }

 private:
  struct CachedGlyph {
    SpriteSheet Sheet;
    glm::vec2 Bearing{0.0f};
    glm::vec2 Size{0.0f};
  };

  CachedGlyph const& GetOrRenderGlyph(uint32_t glyphIndex, uint32_t pixelSize);

  void Reset();

  struct Impl;
  Impl* FontImpl;
};

}  // namespace Impacto::Fonts
