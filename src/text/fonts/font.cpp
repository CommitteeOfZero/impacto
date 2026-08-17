#include "font.h"

#include "../../log.h"
#include "../../profile/game.h"
#include "../../renderer/renderer.h"

namespace Impacto {

float Font::ApplyOpacityCurve(float opacity, OpacityCurve curve) {
  switch (curve) {
    using enum OpacityCurve;
    case Linear:
      return opacity;
    case Smoothstep:
      return glm::smoothstep(0.0f, 1.0f, opacity);
    case CubedTransparency:
      return 1.0f - std::pow(1.0f - opacity, 3.0f);
  }
  assert(false);
  return opacity;
}

std::vector<size_t> Font::GetVisibleGlyphIds(
    std::span<const ProcessedTextGlyph> text) {
  const static RectF viewport{0.0f, 0.0f, Profile::Game::DesignWidth,
                              Profile::Game::DesignHeight};

  std::vector<size_t> visibleGlyphIds;
  for (size_t i = 0; i < text.size(); i++) {
    if (text[i].DestRect.Intersects(viewport)) visibleGlyphIds.push_back(i);
  }
  return visibleGlyphIds;
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

void SingleSheetFont::DrawProcessedText(
    const std::span<const ProcessedTextGlyph> text, const float opacity,
    const float outlineOpacity, const RendererOutlineMode outlineMode,
    const SpriteSheet* const maskedSheet, const glm::mat4 transformation) {
  const std::vector<size_t> visibleGlyphIds = GetVisibleGlyphIds(text);
  const size_t glyphCount = visibleGlyphIds.size();
  if (glyphCount == 0) return;

  const size_t vertexCount = glyphCount * 4;
  const size_t indexCount = glyphCount * 6;
  std::vector<VertexBufferSprites> vertices(vertexCount);
  std::vector<uint16_t> indices;

  FillGlyphIndices(indices, static_cast<uint16_t>(glyphCount));

  for (size_t i = 0; i < glyphCount; i++) {
    const ProcessedTextGlyph glyph = text[visibleGlyphIds[i]];

    const CornersQuad dest = glyph.DestRect;
    const CornersQuad destUV = GetGlyph(glyph.CharId).NormalizedBounds();
    const CornersQuad maskUV = CornersQuad(dest).Scale(
        {1.0f / Window->WindowWidth, 1.0f / Window->WindowHeight},
        {0.0f, 0.0f});
    glm::vec4 color = RgbIntToFloat(glyph.Colors.TextColor);
    color.a =
        ApplyOpacityCurve(glyph.Opacity * opacity, ForegroundOpacityCurve);

    InsertQuad(std::span<VertexBufferSprites, 4>(vertices.begin() + i * 4, 4),
               dest, destUV, color, maskUV);
  }
  uint16_t maxIndex = (uint16_t)vertexCount;

  if (outlineMode != RendererOutlineMode::None) {
    // Add outline to the front of the buffers
    vertices.insert(vertices.end(), vertices.begin(), vertices.end());

    indices.reserve(indexCount * 2);
    std::transform(indices.begin(), indices.end(), std::back_inserter(indices),
                   [maxIndex](uint16_t index) { return index + maxIndex; });
    maxIndex += (uint16_t)vertexCount;

    // Set the color of the outline
    for (size_t i = 0; i < glyphCount; i++) {
      const ProcessedTextGlyph glyph = text[visibleGlyphIds[i]];
      glm::vec4 color = RgbIntToFloat(glyph.Colors.OutlineColor);
      color.a = ApplyOpacityCurve(glyph.Opacity * outlineOpacity,
                                  OutlineOpacityCurve);
      const auto glyphStart = vertices.begin() + i * 4;
      const auto glyphEnd = glyphStart + 4;
      std::transform(glyphStart, glyphEnd, glyphStart, [color](auto vertex) {
        vertex.Tint = color;
        return vertex;
      });
    }

    const auto translateVertex = [](VertexBufferSprites vertex,
                                    const glm::vec2 offset) {
      vertex.Position += offset;
      return vertex;
    };

    switch (outlineMode) {
      case RendererOutlineMode::Full: {
        // Add bottom-right outline
        vertices.insert(vertices.begin() + vertexCount, vertices.begin(),
                        vertices.begin() + vertexCount);

        indices.resize(indexCount * 3);
        std::transform(indices.begin(), indices.begin() + indexCount,
                       indices.begin() + indexCount * 2,
                       [maxIndex](uint16_t index) { return index + maxIndex; });
        maxIndex += (uint16_t)vertexCount;

        // Translate outlines
        const auto tlOutlineStart = vertices.begin();
        const auto brOutlineStart = tlOutlineStart + vertexCount;
        const auto foregroundStart = brOutlineStart + vertexCount;
        std::transform(tlOutlineStart, brOutlineStart, tlOutlineStart,
                       [translateVertex](auto vertex) {
                         return translateVertex(vertex, {-1.0f, -1.0f});
                       });
        std::transform(brOutlineStart, foregroundStart, brOutlineStart,
                       [translateVertex](auto vertex) {
                         return translateVertex(vertex, {1.0f, 1.0f});
                       });

        break;
      }

      case RendererOutlineMode::BottomRight: {
        const auto brOutlineStart = vertices.begin();
        const auto foregroundStart = vertices.begin() + vertexCount;
        std::transform(brOutlineStart, foregroundStart, brOutlineStart,
                       [translateVertex](auto vertex) {
                         return translateVertex(vertex, {1.0f, 1.0f});
                       });

        break;
      }

      default:
        ImpLogSlow(LogLevel::Warning, LogChannel::Render,
                   "Unexpected outline mode!");
        break;
    }
  }

  const ShaderProgramType shader = maskedSheet == nullptr
                                       ? ShaderProgramType::Sprite
                                       : ShaderProgramType::MaskedSpriteNoAlpha;
  Renderer->DrawPrimitives(Sheet, maskedSheet, shader, vertices, indices,
                           transformation);
}

void SeparateOutlineSheetFont::DrawProcessedText(
    const std::span<const ProcessedTextGlyph> text, const float opacity,
    const float outlineOpacity, const RendererOutlineMode outlineMode,
    const SpriteSheet* const maskedSheet, const glm::mat4 transformation) {
  const std::vector<size_t> visibleGlyphIds = GetVisibleGlyphIds(text);
  const uint16_t glyphCount = static_cast<uint16_t>(visibleGlyphIds.size());
  if (glyphCount == 0) return;

  const ShaderProgramType shader = maskedSheet == nullptr
                                       ? ShaderProgramType::Sprite
                                       : ShaderProgramType::MaskedSpriteNoAlpha;

  const size_t vertexCount = glyphCount * 4;
  const size_t indexCount = glyphCount * 6;
  std::vector<VertexBufferSprites> vertices;
  std::vector<uint16_t> indices;

  const auto fillVertices = [&]<auto SeparateOutlineSheetFont::* getGlyphMethod,
                                uint32_t DialogueColorPair::* colorMember>(
                                glm::vec2 offset) {
    const auto [textOpacity, opacityCurve] =
        colorMember == &DialogueColorPair::OutlineColor
            ? std::pair{outlineOpacity, OutlineOpacityCurve}
            : std::pair{opacity, ForegroundOpacityCurve};

    vertices.resize(vertexCount);
    for (size_t i = 0; i < glyphCount; i++) {
      const ProcessedTextGlyph glyph = text[visibleGlyphIds[i]];

      const CornersQuad dest = glyph.DestRect + offset;
      const CornersQuad destUV =
          (this->*getGlyphMethod)(glyph.CharId).NormalizedBounds();
      const CornersQuad maskUV = CornersQuad(dest).Scale(
          {1.0f / Window->WindowWidth, 1.0f / Window->WindowHeight},
          {0.0f, 0.0f});
      glm::vec4 color = RgbIntToFloat(glyph.Colors.*colorMember);
      color.a = ApplyOpacityCurve(glyph.Opacity * textOpacity, opacityCurve);

      InsertQuad(std::span<VertexBufferSprites, 4>(vertices.begin() + i * 4, 4),
                 dest, destUV, color, maskUV);
    }
  };

  if (outlineMode != RendererOutlineMode::None) {
    switch (outlineMode) {
      using enum RendererOutlineMode;
      case Full: {
        vertices.reserve(vertexCount * 2);
        FillGlyphIndices(indices, glyphCount * 2);

        fillVertices.template
        operator()<&SeparateOutlineSheetFont::GetOutlineGlyph,
                   &DialogueColorPair::OutlineColor>({0.0f, 0.0f});

        Renderer->DrawPrimitives(
            OutlineSheet, maskedSheet, shader, vertices,
            std::span(indices.begin(), indices.begin() + indexCount),
            transformation);

        fillVertices.template operator()<&SeparateOutlineSheetFont::GetGlyph,
                                         &DialogueColorPair::OutlineColor>(
            {-1.0f, -1.0f});

        const auto translateVertex = [](VertexBufferSprites vertex,
                                        const glm::vec2 offset) {
          vertex.Position += offset;
          return vertex;
        };

        std::transform(vertices.begin(), vertices.end(),
                       std::back_inserter(vertices),
                       [translateVertex](auto vertex) {
                         return translateVertex(vertex, {2.0f, 2.0f});
                       });

        Renderer->DrawPrimitives(ForegroundSheet, maskedSheet, shader, vertices,
                                 indices, transformation);

        indices.resize(indexCount);
      } break;

      case BottomRight: {
        fillVertices.template
        operator()<&SeparateOutlineSheetFont::GetOutlineGlyph,
                   &DialogueColorPair::OutlineColor>({1.0f, 1.0f});

        FillGlyphIndices(indices, glyphCount);

        Renderer->DrawPrimitives(OutlineSheet, maskedSheet, shader, vertices,
                                 indices, transformation);
      } break;

      case None: {
        assert(false);
      } break;
    }

  } else {
    FillGlyphIndices(indices, glyphCount);
  }

  fillVertices.template operator()<&SeparateOutlineSheetFont::GetGlyph,
                                   &DialogueColorPair::TextColor>(
      glm::vec2(0.0f));

  Renderer->DrawPrimitives(ForegroundSheet, maskedSheet, shader, vertices,
                           indices, transformation);
}

void LanguageBarrierFont::DrawProcessedText(
    const std::span<const ProcessedTextGlyph> text, const float opacity,
    const float outlineOpacity, const RendererOutlineMode outlineMode,
    const SpriteSheet* const maskedSheet, const glm::mat4 transformation) {
  const std::vector<size_t> visibleGlyphIds = GetVisibleGlyphIds(text);
  const size_t glyphCount = visibleGlyphIds.size();
  if (glyphCount == 0) return;

  const ShaderProgramType shader = maskedSheet == nullptr
                                       ? ShaderProgramType::Sprite
                                       : ShaderProgramType::MaskedSpriteNoAlpha;

  const size_t vertexCount = glyphCount * 4;
  const size_t indexCount = glyphCount * 6;
  std::vector<VertexBufferSprites> vertices(vertexCount);
  std::vector<uint16_t> indices(indexCount);

  FillGlyphIndices(indices, static_cast<uint16_t>(glyphCount));

  if (outlineMode != RendererOutlineMode::None) {
    for (size_t i = 0; i < glyphCount; i++) {
      const ProcessedTextGlyph glyph = text[visibleGlyphIds[i]];

      glm::vec4 color = RgbIntToFloat(glyph.Colors.OutlineColor);
      color.a = ApplyOpacityCurve(glyph.Opacity * outlineOpacity,
                                  OutlineOpacityCurve);
      const CornersQuad destUV =
          GetOutlineGlyph(glyph.CharId).NormalizedBounds();

      CornersQuad dest = RectF();
      const glm::vec2 scale = {glyph.DestRect.Height / BitmapEmWidth,
                               glyph.DestRect.Height / BitmapEmHeight};
      switch (outlineMode) {
        case RendererOutlineMode::Full: {
          dest = RectF(OutlineOffset.x, OutlineOffset.y, OutlineCellSize.x,
                       OutlineCellSize.y)
                     .Scale(scale, {0.0f, 0.0f})
                     .Translate(glyph.DestRect.GetPos());
          break;
        }

        case RendererOutlineMode::BottomRight: {
          dest = RectF(OutlineOffset.x * 3 / 4, OutlineOffset.y * 3 / 4,
                       OutlineCellSize.x + OutlineOffset.x / 2,
                       OutlineCellSize.y + OutlineOffset.y / 2)
                     .Scale(scale, {0.0f, 0.0f})
                     .Translate(glyph.DestRect.GetPos());
          break;
        }

        default:
          ImpLogSlow(LogLevel::Warning, LogChannel::Render,
                     "Unexpected outline mode!");
          break;
      }

      const CornersQuad maskUV =
          CornersQuad(dest).Scale({1.0f / Profile::Game::DesignWidth,
                                   1.0f / Profile::Game::DesignHeight},
                                  {0.0f, 0.0f});
      InsertQuad(std::span<VertexBufferSprites, 4>(vertices.begin() + i * 4, 4),
                 dest, destUV, color, maskUV);
    }

    Renderer->DrawPrimitives(OutlineSheet, maskedSheet, shader, vertices,
                             indices, transformation);
  }

  for (size_t i = 0; i < glyphCount; i++) {
    const ProcessedTextGlyph glyph = text[visibleGlyphIds[i]];

    glm::vec2 scale = {glyph.DestRect.Height / BitmapEmWidth,
                       glyph.DestRect.Height / BitmapEmHeight};
    CornersQuad dest = RectF(ForegroundOffset.x, ForegroundOffset.y,
                             ForegroundCellSize.x, ForegroundCellSize.y)
                           .Scale(scale, {0.0f, 0.0f})
                           .Translate(glyph.DestRect.GetPos());

    const CornersQuad destUV = GetGlyph(glyph.CharId).NormalizedBounds();

    glm::vec4 color = RgbIntToFloat(glyph.Colors.TextColor);
    color.a =
        ApplyOpacityCurve(glyph.Opacity * opacity, ForegroundOpacityCurve);

    const CornersQuad maskUV = CornersQuad(dest).Scale(
        {1.0f / Profile::Game::DesignWidth, 1.0f / Profile::Game::DesignHeight},
        {0.0f, 0.0f});
    InsertQuad(std::span<VertexBufferSprites, 4>(vertices.begin() + i * 4, 4),
               dest, destUV, color, maskUV);
  }

  Renderer->DrawPrimitives(ForegroundSheet, maskedSheet, shader, vertices,
                           indices, transformation);
}

void EdgeDetectedSingleSheetFont::DrawProcessedText(
    const std::span<const ProcessedTextGlyph> text, const float opacity,
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
      const CornersQuad destUV = GetGlyph(glyph.CharId).NormalizedBounds();
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
      alphaShift = 0.1f;

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
      GetGlyph(text.begin()->CharId).Bounds.GetSize() /
      text.begin()->DestRect.GetSize();

  Renderer->DrawEdgeDetectedSingleSheetFont(
      Sheet, maskedSheet, vertices, indices, intensityShift, alphaShift,
      renderScale, transformation, glm::mat4(1.0f));
}

}  // namespace Impacto
