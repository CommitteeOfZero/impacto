#include "font.h"

#include "../../io/physicalfilestream.h"
#include "../../renderer/renderer.h"

namespace Impacto::Fonts {

EdgeDetectedSingleVariableWidthSheetFont::
    EdgeDetectedSingleVariableWidthSheetFont(float bitmapEmWidth,
                                             float bitmapEmHeight,
                                             OpacityCurve opacityCurve,
                                             SpriteSheet sheet,
                                             Io::AssetPath& binaryPath)
    : Font(FontType::EdgeDetectedSingleVariableWidthSheet, bitmapEmWidth,
           bitmapEmHeight, opacityCurve, opacityCurve) {
  const std::unique_ptr<Io::Stream> stream([&binaryPath]() {
    Io::Stream* streamPtr;
    const IoError err = binaryPath.Open(&streamPtr);

    switch (err) {
      case IoError::IoError_NotFound:
        throw std::runtime_error(
            fmt::format("Could not find EdgeDetectedSingleVariableWidthSheet "
                        "font binary file"));
      case IoError::IoError_Fail:
      case IoError::IoError_Eof:
        throw std::runtime_error(
            fmt::format("Failed to open EdgeDetectedSingleVariableWidthSheet "
                        "font binary file"));
      case IoError::IoError_OK:
        break;
    }

    return streamPtr;
  }());

  constexpr size_t glyphDataSize = 8 * sizeof(int16_t);
  assert(stream->Meta.Size % glyphDataSize == 0);
  const size_t glyphCount = stream->Meta.Size / glyphDataSize;

  Data.reserve(glyphCount);
  for (size_t idx = 0; idx < glyphCount; idx++) {
    const int16_t xOffset = Io::ReadLE<int16_t>(stream.get());
    const int16_t yOffset = Io::ReadLE<int16_t>(stream.get());
    const uint16_t posX = Io::ReadLE<uint16_t>(stream.get());
    const uint16_t posY = Io::ReadLE<uint16_t>(stream.get());
    const uint16_t width = Io::ReadLE<uint16_t>(stream.get());
    const uint16_t height = Io::ReadLE<uint16_t>(stream.get());
    const int16_t advanceWidth = Io::ReadLE<int16_t>(stream.get());
    const int16_t advanceHeight = Io::ReadLE<int16_t>(stream.get());

    Data.emplace_back(GlyphData{
        .GlyphSprite =
            Sprite(sheet, static_cast<float>(posX), static_cast<float>(posY),
                   static_cast<float>(width), static_cast<float>(height)),
        .Offset = {static_cast<float>(xOffset), static_cast<float>(yOffset)},
        .AdvanceSize = {static_cast<float>(advanceWidth),
                        static_cast<float>(advanceHeight)},
    });
  }
}

static void FillGlyphIndices(std::vector<uint16_t>& indices,
                             uint16_t glyphCount) {
  indices.resize(glyphCount * 6);
  for (uint16_t i = 0; i < glyphCount; i++) {
    const uint16_t bl = i * 4;
    const uint16_t tl = bl + 1;
    const uint16_t tr = bl + 2;
    const uint16_t br = bl + 3;

    indices[i * 6 + 0] = bl;
    indices[i * 6 + 1] = tl;
    indices[i * 6 + 2] = tr;
    indices[i * 6 + 3] = bl;
    indices[i * 6 + 4] = tr;
    indices[i * 6 + 5] = br;
  }
}

void EdgeDetectedSingleVariableWidthSheetFont::DrawProcessedText(
    std::span<const ProcessedTextGlyph> text, const float opacity,
    const float outlineOpacity, const RendererOutlineMode outlineMode,
    const SpriteSheet* const maskedSheet, const glm::mat4 transformation) {
  const std::vector<size_t> visibleGlyphIds = GetVisibleGlyphIds(text);
  const size_t glyphCount = visibleGlyphIds.size();
  if (glyphCount == 0) return;

  const size_t vertexCount = glyphCount * 4;
  const size_t indexCount = glyphCount * 6;
  std::vector<VertexBufferSprites> vertices;
  std::vector<uint16_t> indices(indexCount);

  const auto insertVertices = [&](uint32_t DialogueColorPair::* colorMember,
                                  glm::vec2 offset) {
    const auto [textOpacity, opacityCurve] =
        colorMember == &DialogueColorPair::OutlineColor
            ? std::pair{outlineOpacity, OutlineOpacityCurve}
            : std::pair{opacity, ForegroundOpacityCurve};

    const size_t prevVertexCount = vertices.size();
    vertices.resize(prevVertexCount + vertexCount);
    const auto newVerticesBegin = vertices.begin() + prevVertexCount;
    for (size_t i = 0; i < glyphCount; i++) {
      const ProcessedTextGlyph glyph = text[visibleGlyphIds[i]];

      const CornersQuad dest = glyph.DestRect + offset;
      const CornersQuad destUV =
          Data[glyph.CharId].GlyphSprite.NormalizedBounds();
      const CornersQuad maskUV = CornersQuad(dest).Scale(
          {1.0f / Window->WindowWidth, 1.0f / Window->WindowHeight},
          {0.0f, 0.0f});
      glm::vec4 color = RgbIntToFloat(glyph.Colors.*colorMember);
      color.a = ApplyOpacityCurve(glyph.Opacity * textOpacity, opacityCurve);

      InsertQuad(std::span<VertexBufferSprites, 4>(newVerticesBegin + i * 4, 4),
                 dest, destUV, color, maskUV);
    }
  };

  float intensityShift = 0.5f;
  float alphaShift = 0.1f;

  switch (outlineMode) {
    using enum RendererOutlineMode;
    case None: {
      intensityShift = -2.0f;
      alphaShift = 0.5f;

      FillGlyphIndices(indices, static_cast<uint16_t>(glyphCount));
    } break;

    case Full: {
      intensityShift = 0.5f;
      alphaShift = 0.3f;

      FillGlyphIndices(indices, static_cast<uint16_t>(glyphCount));
    } break;

    case BottomRight: {
      intensityShift = -2.0f;
      alphaShift = 0.5f;

      FillGlyphIndices(indices, static_cast<uint16_t>(glyphCount * 2));
      insertVertices(&DialogueColorPair::OutlineColor, {1.0f, 1.0f});
    } break;
  }

  insertVertices(&DialogueColorPair::TextColor, {0.0f, 0.0f});

  const glm::vec2 renderScale =
      Data[text.begin()->CharId].GlyphSprite.Bounds.GetSize() /
      text.begin()->DestRect.GetSize();

  Renderer->DrawEdgeDetectedSingleSheetFont(
      Data.front().GlyphSprite.Sheet, maskedSheet, vertices, indices,
      intensityShift, alphaShift, renderScale, transformation, glm::mat4(1.0f));
}

}  // namespace Impacto::Fonts
