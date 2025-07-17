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

#include <numeric>

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

static void InsertQuad(const std::span<VertexBufferSprites, 4> vertices,
                       const CornersQuad position, const CornersQuad uv,
                       const glm::vec4 tint,
                       const CornersQuad maskUV = RectF()) {
  vertices[0] = {
      .Position = position.BottomLeft,
      .UV = uv.BottomLeft,
      .Tint = tint,
      .MaskUV = maskUV.BottomLeft,
  };
  vertices[1] = {
      .Position = position.TopLeft,
      .UV = uv.TopLeft,
      .Tint = tint,
      .MaskUV = maskUV.TopLeft,
  };
  vertices[2] = {
      .Position = position.TopRight,
      .UV = uv.TopRight,
      .Tint = tint,
      .MaskUV = maskUV.TopRight,
  };
  vertices[3] = {
      .Position = position.BottomRight,
      .UV = uv.BottomRight,
      .Tint = tint,
      .MaskUV = maskUV.BottomRight,
  };
}

void BaseRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                    glm::vec2 topLeft, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    glm::vec2 scale) {
  const RectF dest =
      sprite.ScaledBounds().Scale(scale, {0.0f, 0.0f}).Translate(topLeft);
  DrawCCMessageBox(sprite, mask, dest, tint, alpha, fadeRange, effectCt);
}

void BaseRenderer::DrawConvexShape(const std::span<const glm::vec2> vertices,
                                   const glm::mat4 transformation,
                                   const glm::vec4 color) {
  assert(vertices.size() >= 3);

  if (vertices.size() == 4) {
    CornersQuad quad = {vertices[0], vertices[1], vertices[2], vertices[3]};
    quad.Transform(transformation);
    DrawQuad(quad, color);

    return;
  }

  std::vector<VertexBufferSprites> vertexAttributes;
  vertexAttributes.reserve(vertices.size());
  std::transform(
      vertices.begin(), vertices.end(), std::back_inserter(vertexAttributes),
      [color, transformation](const glm::vec2 pos) {
        return VertexBufferSprites{
            .Position = glm::vec2(transformation * glm::vec4(pos, 0.0f, 1.0f)),
            .Tint = color,
        };
      });

  if (vertices.size() == 3) {
    DrawVertices(RectSprite.Sheet, ShaderProgramType::Sprite, vertexAttributes,
                 std::array<uint16_t, 3>{0, 1, 2});
    return;
  }

  std::vector<uint16_t> indices;
  indices.reserve(vertices.size() * 3);

  const glm::vec2 centerOfMass =
      std::reduce(vertices.begin(), vertices.end()) / (float)vertices.size();

  const uint16_t centerOfMassIndex = (uint16_t)vertices.size();
  vertexAttributes.emplace_back(VertexBufferSprites{
      .Position = centerOfMass,
      .Tint = color,
  });

  for (uint16_t i = 0; i < centerOfMassIndex; i++) {
    const uint16_t nextIndex = (i != centerOfMassIndex - 1) ? i + 1 : 0;
    indices.insert(indices.end(), {centerOfMassIndex, i, nextIndex});
  }

  DrawVertices(RectSprite.Sheet, ShaderProgramType::Sprite, vertexAttributes,
               indices);
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
  vertices.resize(vertexCount);
  indices.resize(indexCount);

  for (size_t i = 0; i < text.size(); i++) {
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

  for (size_t i = 0; i < text.size(); i++) {
    const ProcessedTextGlyph glyph = text[i];

    const CornersQuad dest = glyph.DestRect;
    const CornersQuad destUV = font->Glyph(glyph.CharId).NormalizedBounds();
    const CornersQuad maskUV = CornersQuad(dest).Scale(
        {1.0f / Window->WindowWidth, 1.0f / Window->WindowHeight},
        {0.0f, 0.0f});
    glm::vec4 color = RgbIntToFloat(glyph.Colors.TextColor);
    color.a = opacity * (smoothstepGlyphOpacity
                             ? glm::smoothstep(0.0f, 1.0f, glyph.Opacity)
                             : glyph.Opacity);

    InsertQuad(std::span<VertexBufferSprites, 4>(vertices.begin() + i * 4, 4),
               dest, destUV, color, maskUV);
  }
  uint16_t maxIndex = vertexCount;

  if (outlineMode != RendererOutlineMode::None) {
    // Add outline to the front of the buffers
    vertices.insert(vertices.end(), vertices.begin(), vertices.end());

    indices.reserve(indexCount * 2);
    std::transform(indices.begin(), indices.end(), std::back_inserter(indices),
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
        maxIndex += vertexCount;

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
  DrawVertices(font->Sheet, maskedSheet, shader, vertices, indices);
}

void BaseRenderer::DrawProcessedText_LBFont(
    std::span<const ProcessedTextGlyph> text, LBFont* font, float opacity,
    RendererOutlineMode outlineMode, bool smoothstepGlyphOpacity,
    float outlineOpacity, SpriteSheet* maskedSheet) {
  const ShaderProgramType shader = maskedSheet == nullptr
                                       ? ShaderProgramType::Sprite
                                       : ShaderProgramType::MaskedSpriteNoAlpha;

  const size_t vertexCount = text.size() * 4;
  const size_t indexCount = text.size() * 6;
  std::vector<VertexBufferSprites> vertices(vertexCount);
  std::vector<uint16_t> indices(indexCount);

  for (size_t i = 0; i < text.size(); i++) {
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

  if (outlineMode != RendererOutlineMode::None) {
    for (size_t i = 0; i < text.size(); i++) {
      const ProcessedTextGlyph glyph = text[i];

      glm::vec4 color = RgbIntToFloat(glyph.Colors.OutlineColor);
      color.a =
          outlineOpacity * (smoothstepGlyphOpacity
                                ? glm::smoothstep(0.0f, 1.0f, glyph.Opacity)
                                : glyph.Opacity);
      const CornersQuad destUV =
          font->OutlineGlyph(glyph.CharId).NormalizedBounds();

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

      const CornersQuad maskUV = CornersQuad(dest).Scale(
          {1.0f / Window->WindowWidth, 1.0f / Window->WindowHeight},
          {0.0f, 0.0f});
      InsertQuad(std::span<VertexBufferSprites, 4>(vertices.begin() + i * 4, 4),
                 dest, destUV, color, maskUV);
    }

    DrawVertices(font->OutlineSheet, maskedSheet, shader, vertices, indices);
  }

  for (size_t i = 0; i < text.size(); i++) {
    const ProcessedTextGlyph glyph = text[i];

    glm::vec2 scale = {glyph.DestRect.Height / font->BitmapEmWidth,
                       glyph.DestRect.Height / font->BitmapEmHeight};
    CornersQuad dest = RectF(font->ForegroundOffset.x, font->ForegroundOffset.y,
                             font->CellWidth, font->CellHeight)
                           .Scale(scale, {0.0f, 0.0f})
                           .Translate(glyph.DestRect.GetPos());

    const CornersQuad destUV = font->Glyph(glyph.CharId).NormalizedBounds();

    glm::vec4 color = RgbIntToFloat(glyph.Colors.TextColor);
    color.a = opacity * (smoothstepGlyphOpacity
                             ? glm::smoothstep(0.0f, 1.0f, glyph.Opacity)
                             : glyph.Opacity);

    const CornersQuad maskUV = CornersQuad(dest).Scale(
        {1.0f / Window->WindowWidth, 1.0f / Window->WindowHeight},
        {0.0f, 0.0f});
    InsertQuad(std::span<VertexBufferSprites, 4>(vertices.begin() + i * 4, 4),
               dest, destUV, color, maskUV);
  }

  DrawVertices(font->ForegroundSheet, maskedSheet, shader, vertices, indices);
}

void BaseRenderer::QuadSetPosition(CornersQuad quad, glm::vec2* const pos,
                                   int stride) {
  *(glm::vec2*)((uint8_t*)pos + 0 * stride) = DesignToNDC(quad.TopLeft);
  *(glm::vec2*)((uint8_t*)pos + 1 * stride) = DesignToNDC(quad.BottomLeft);
  *(glm::vec2*)((uint8_t*)pos + 2 * stride) = DesignToNDC(quad.BottomRight);
  *(glm::vec2*)((uint8_t*)pos + 3 * stride) = DesignToNDC(quad.TopRight);
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
