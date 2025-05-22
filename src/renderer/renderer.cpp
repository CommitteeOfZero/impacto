#include "renderer.h"

#include "../profile/game.h"

#ifndef IMPACTO_DISABLE_OPENGL
#include "opengl/renderer.h"
#endif
#ifndef IMPACTO_DISABLE_VULKAN
#include "vulkan/renderer.h"
#endif
#ifndef IMPACTO_DISABLE_DX9
#include "dx9/renderer.h"
#endif

namespace Impacto {

void InitRenderer() {
  switch (Profile::ActiveRenderer) {
#ifndef IMPACTO_DISABLE_OPENGL
    case RendererType::OpenGL:
      Renderer = new OpenGL::Renderer();
      break;
#endif
#ifndef IMPACTO_DISABLE_VULKAN
    case RendererType::Vulkan:
      Renderer = new Vulkan::Renderer();
      break;
#endif
#ifndef IMPACTO_DISABLE_DX9
    case RendererType::DirectX9:
      Renderer = new DirectX9::Renderer();
      break;
#endif
    default:
      ImpLog(LogLevel::Error, LogChannel::Render,
             "Unknown or unsupported renderer selected!\n");
      exit(1);
  }

  Renderer->Init();
}

static void InsertQuad(std::vector<VertexBufferSprites>& vertices,
                       const CornersQuad position, const CornersQuad uv,
                       const glm::vec4 tint,
                       const CornersQuad maskUV = RectF()) {
  vertices.insert(vertices.end(), {
                                      VertexBufferSprites{
                                          .Position = position.TopLeft,
                                          .UV = uv.TopLeft,
                                          .Tint = tint,
                                          .MaskUV = maskUV.TopLeft,
                                      },
                                      VertexBufferSprites{
                                          .Position = position.TopRight,
                                          .UV = uv.TopRight,
                                          .Tint = tint,
                                          .MaskUV = maskUV.TopRight,
                                      },
                                      VertexBufferSprites{
                                          .Position = position.BottomRight,
                                          .UV = uv.BottomRight,
                                          .Tint = tint,
                                          .MaskUV = maskUV.BottomRight,
                                      },
                                      VertexBufferSprites{
                                          .Position = position.BottomLeft,
                                          .UV = uv.BottomLeft,
                                          .Tint = tint,
                                          .MaskUV = maskUV.BottomLeft,
                                      },
                                  });
}

static void InsertQuad(std::vector<VertexBufferSprites>& vertices,
                       std::vector<uint16_t>& indices, uint16_t& maxIndex,
                       const CornersQuad position, const CornersQuad uv,
                       const glm::vec4 tint,
                       const CornersQuad maskUV = RectF()) {
  InsertQuad(vertices, position, uv, tint, maskUV);

  const uint16_t tl = maxIndex;
  const uint16_t tr = maxIndex + 1;
  const uint16_t br = maxIndex + 2;
  const uint16_t bl = maxIndex + 3;
  indices.insert(indices.end(), {tl, tr, br, tl, br, bl});
  maxIndex += 4;
}

void BaseRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                    glm::vec2 topLeft, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    glm::vec2 scale) {
  const RectF dest =
      sprite.ScaledBounds().Scale(scale, {0.0f, 0.0f}).Translate(topLeft);
  DrawCCMessageBox(sprite, mask, dest, tint, alpha, fadeRange, effectCt);
}

void BaseRenderer::DrawQuad(const CornersQuad& dest, const glm::vec4 color) {
  DrawSprite(RectSprite, dest, color);
}

void BaseRenderer::DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                                     Font* font, float opacity,
                                     RendererOutlineMode outlineMode,
                                     bool smoothstepGlyphOpacity,
                                     SpriteSheet* maskedSheet) {
  switch (font->Type) {
    case FontType::Basic:
      DrawProcessedText_BasicFont(text, (BasicFont*)font, opacity, outlineMode,
                                  smoothstepGlyphOpacity, opacity, maskedSheet);
      break;
    case FontType::LB: {
      DrawProcessedText_LBFont(text, (LBFont*)font, opacity, outlineMode,
                               smoothstepGlyphOpacity, opacity, maskedSheet);
    }
  }
}

void BaseRenderer::DrawProcessedText(std::span<const ProcessedTextGlyph> text,
                                     Font* font, float opacity,
                                     float outlineOpacity,
                                     RendererOutlineMode outlineMode,
                                     bool smoothstepGlyphOpacity,
                                     SpriteSheet* maskedSheet) {
  switch (font->Type) {
    case FontType::Basic:
      DrawProcessedText_BasicFont(text, (BasicFont*)font, opacity, outlineMode,
                                  smoothstepGlyphOpacity, outlineOpacity,
                                  maskedSheet);
      break;
    case FontType::LB: {
      DrawProcessedText_LBFont(text, (LBFont*)font, opacity, outlineMode,
                               smoothstepGlyphOpacity, outlineOpacity,
                               maskedSheet);
    }
  }
}

void BaseRenderer::DrawProcessedText_BasicFont(
    std::span<const ProcessedTextGlyph> text, BasicFont* font, float opacity,
    RendererOutlineMode outlineMode, bool smoothstepGlyphOpacity,
    float outlineOpacity, SpriteSheet* maskedSheet) {
  const size_t vertexCount = text.size() * 4;
  const size_t indexCount = text.size() * 6;
  std::vector<VertexBufferSprites> vertices;
  std::vector<uint16_t> indices;
  vertices.reserve(vertexCount);
  indices.reserve(indexCount);

  uint16_t maxIndex = 0;
  for (const ProcessedTextGlyph glyph : text) {
    const CornersQuad dest = glyph.DestRect;
    const CornersQuad destUV = CornersQuad(font->Glyph(glyph.CharId).Bounds)
                                   .Scale({1.0f / font->Sheet.DesignWidth,
                                           1.0f / font->Sheet.DesignHeight},
                                          {0.0f, 0.0f});
    glm::vec4 color = RgbIntToFloat(glyph.Colors.TextColor);
    color.a = opacity * (smoothstepGlyphOpacity
                             ? glm::smoothstep(0.0f, 1.0f, glyph.Opacity)
                             : glyph.Opacity);

    InsertQuad(vertices, indices, maxIndex, dest, destUV, color, dest);
  }

  if (outlineMode != RendererOutlineMode::None) {
    // Add outline to the front of the buffers
    vertices.reserve(vertexCount * 2);
    vertices.insert(vertices.end(), vertices.begin(), vertices.end());

    indices.resize(indexCount * 2);
    std::transform(indices.begin(), indices.begin() + indexCount,
                   indices.begin() + indexCount,
                   [maxIndex](uint16_t index) { return index + maxIndex; });
    maxIndex += vertexCount;

    // Set the color of the outline
    for (size_t i = 0; i < text.size(); i++) {
      const ProcessedTextGlyph glyph = text[i];
      glm::vec4 color = RgbIntToFloat(glyph.Colors.OutlineColor);
      color.a =
          outlineOpacity * (smoothstepGlyphOpacity
                                ? glm::smoothstep(0.0f, 1.0f, glyph.Opacity)
                                : glyph.Opacity);
      std::transform(vertices.begin() + i * 4, vertices.begin() + (i + 1) * 4,
                     vertices.begin() + i * 4, [color](auto vertex) {
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
        vertices.reserve(vertexCount * 3);
        vertices.insert(vertices.begin() + vertexCount, vertices.begin(),
                        vertices.begin() + vertexCount);

        indices.resize(indexCount * 3);
        std::transform(indices.begin(), indices.begin() + indexCount,
                       indices.begin() + indexCount * 2,
                       [maxIndex](uint16_t index) { return index + maxIndex; });
        maxIndex += vertexCount;

        // Translate outlines
        const auto glyphsBegin = vertices.begin() + vertexCount * 2;
        const auto glyphsEnd = vertices.end();
        std::transform(glyphsBegin, glyphsEnd, vertices.begin(),
                       [translateVertex](auto vertex) {
                         return translateVertex(vertex, {-1.0f, -1.0f});
                       });
        std::transform(glyphsBegin, glyphsEnd, vertices.begin() + vertexCount,
                       [translateVertex](auto vertex) {
                         return translateVertex(vertex, {1.0f, 1.0f});
                       });

        break;
      }

      case RendererOutlineMode::BottomRight: {
        std::transform(vertices.begin(), vertices.end(), vertices.begin(),
                       [translateVertex](auto vertex) {
                         return translateVertex(vertex, {-1.0f, -1.0f});
                       });

        break;
      }

      default:
        ImpLogSlow(LogLevel::Warning, LogChannel::Render,
                   "Unexpected outline mode!");
        break;
    }
  }

  DrawVertices(font->Sheet, vertices, indices);
}

void BaseRenderer::DrawProcessedText_LBFont(
    std::span<const ProcessedTextGlyph> text, LBFont* font, float opacity,
    RendererOutlineMode outlineMode, bool smoothstepGlyphOpacity,
    float outlineOpacity, SpriteSheet* maskedSheet) {
  const size_t vertexCount = text.size() * 4;
  const size_t indexCount = text.size() * 6;
  std::vector<uint16_t> indices;
  indices.reserve(indexCount);

  for (size_t i = 0; i < text.size(); i++) {
    const uint16_t tl = i * 4;
    const uint16_t tr = tl + 1;
    const uint16_t br = tl + 2;
    const uint16_t bl = tl + 3;

    indices.insert(indices.end(), {tl, tr, br, tl, br, bl});
  }

  if (outlineMode != RendererOutlineMode::None) {
    std::vector<VertexBufferSprites> outlineVertices;
    outlineVertices.reserve(vertexCount);

    for (const ProcessedTextGlyph glyph : text) {
      glm::vec4 color = RgbIntToFloat(glyph.Colors.OutlineColor);
      color.a =
          outlineOpacity * (smoothstepGlyphOpacity
                                ? glm::smoothstep(0.0f, 1.0f, glyph.Opacity)
                                : glyph.Opacity);
      const CornersQuad destUV =
          CornersQuad(font->OutlineGlyph(glyph.CharId).Bounds)
              .Scale({1.0f / font->OutlineSheet.DesignWidth,
                      1.0f / font->OutlineSheet.DesignHeight},
                     {0.0f, 0.0f});

      CornersQuad dest = RectF();
      const glm::vec2 scale = {glyph.DestRect.Height / font->BitmapEmWidth,
                               glyph.DestRect.Height / font->BitmapEmHeight};
      switch (outlineMode) {
        case RendererOutlineMode::Full: {
          dest = RectF(font->OutlineOffset.x, font->OutlineOffset.y,
                       font->OutlineCellWidth, font->OutlineCellHeight)
                     .Scale(scale, {0.0f, 0.0f})
                     .Translate(glyph.DestRect.GetPos());
          break;
        }

        case RendererOutlineMode::BottomRight: {
          dest = RectF(font->OutlineOffset.x * 3 / 4,
                       font->OutlineOffset.y * 3 / 4,
                       font->OutlineCellWidth + font->OutlineOffset.x / 2,
                       font->OutlineCellHeight + font->OutlineOffset.y / 2)
                     .Scale(scale, {0.0f, 0.0f})
                     .Translate(glyph.DestRect.GetPos());
          break;
        }

        default:
          ImpLogSlow(LogLevel::Warning, LogChannel::Render,
                     "Unexpected outline mode!");
          break;
      }

      InsertQuad(outlineVertices, dest, destUV, color, dest);
    }

    DrawVertices(font->OutlineSheet, outlineVertices, indices);
  }

  std::vector<VertexBufferSprites> vertices;
  vertices.reserve(vertexCount);

  for (const ProcessedTextGlyph glyph : text) {
    glm::vec2 scale = {glyph.DestRect.Height / font->BitmapEmWidth,
                       glyph.DestRect.Height / font->BitmapEmHeight};
    CornersQuad dest = RectF(font->ForegroundOffset.x, font->ForegroundOffset.y,
                             font->CellWidth, font->CellHeight)
                           .Scale(scale, {0.0f, 0.0f})
                           .Translate(glyph.DestRect.GetPos());

    const CornersQuad destUV =
        CornersQuad(font->Glyph(glyph.CharId).Bounds)
            .Scale({1.0f / font->OutlineSheet.DesignWidth,
                    1.0f / font->OutlineSheet.DesignHeight},
                   {0.0f, 0.0f});

    glm::vec4 color = RgbIntToFloat(glyph.Colors.TextColor);
    color.a = opacity * (smoothstepGlyphOpacity
                             ? glm::smoothstep(0.0f, 1.0f, glyph.Opacity)
                             : glyph.Opacity);

    InsertQuad(vertices, dest, destUV, color, dest);
  }

  DrawVertices(font->ForegroundSheet, vertices, indices);

  return;
}

void BaseRenderer::QuadSetPosition(CornersQuad quad, glm::vec2* const pos,
                                   int stride) {
  *(glm::vec2*)((uint8_t*)pos + 0 * stride) = quad.TopLeft;
  *(glm::vec2*)((uint8_t*)pos + 1 * stride) = quad.BottomLeft;
  *(glm::vec2*)((uint8_t*)pos + 2 * stride) = quad.BottomRight;
  *(glm::vec2*)((uint8_t*)pos + 3 * stride) = quad.TopRight;
}

void BaseRenderer::QuadSetUV(const CornersQuad bounds,
                             const glm::vec2 dimensions, glm::vec2* const uvs,
                             const size_t stride) {
  *(glm::vec2*)((uint8_t*)uvs + 0 * stride) = bounds.TopLeft / dimensions;
  *(glm::vec2*)((uint8_t*)uvs + 1 * stride) = bounds.BottomLeft / dimensions;
  *(glm::vec2*)((uint8_t*)uvs + 2 * stride) = bounds.BottomRight / dimensions;
  *(glm::vec2*)((uint8_t*)uvs + 3 * stride) = bounds.TopRight / dimensions;
}

}  // namespace Impacto
