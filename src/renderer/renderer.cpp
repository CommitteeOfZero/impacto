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

void BaseRenderer::DrawSprite(const Sprite& sprite,
                              const glm::mat4 transformation,
                              const glm::vec4 tint, const bool inverted) {
  const CornersQuad dest = sprite.ScaledBounds().Transform(transformation);
  DrawSprite(sprite, dest, tint, inverted);
}

void BaseRenderer::DrawSprite(const Sprite& sprite, const glm::vec2 topLeft,
                              const glm::vec4 tint, const bool inverted) {
  const CornersQuad dest = sprite.ScaledBounds().Translate(topLeft);
  DrawSprite(sprite, dest, tint, inverted);
}

void BaseRenderer::DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                                    const int alpha, const int fadeRange,
                                    const glm::mat4 transformation,
                                    const glm::vec4 tint, const bool isInverted,
                                    const bool isSameTexture) {
  const CornersQuad dest = sprite.ScaledBounds().Transform(transformation);
  DrawMaskedSprite(sprite, mask, dest, alpha, fadeRange, tint, isInverted,
                   isSameTexture);
}

void BaseRenderer::DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                                    const int alpha, const int fadeRange,
                                    const glm::vec2 topLeft,
                                    const glm::vec4 tint, const bool isInverted,
                                    const bool isSameTexture) {
  const RectF dest = sprite.ScaledBounds().Translate(topLeft);
  DrawMaskedSprite(sprite, mask, dest, alpha, fadeRange, tint, isInverted,
                   isSameTexture);
}

void BaseRenderer::DrawMaskedSpriteOverlay(
    const Sprite& sprite, const Sprite& mask, const int alpha,
    const int fadeRange, const glm::mat4 transformation, const glm::vec4 tint,
    const bool isInverted, const bool useMaskAlpha) {
  const CornersQuad dest = sprite.ScaledBounds().Transform(transformation);
  DrawMaskedSpriteOverlay(sprite, mask, dest, alpha, fadeRange, tint,
                          isInverted, useMaskAlpha);
}

void BaseRenderer::DrawMaskedSpriteOverlay(
    const Sprite& sprite, const Sprite& mask, const int alpha,
    const int fadeRange, const glm::vec2 topLeft, const glm::vec4 tint,
    const bool isInverted, const bool useMaskAlpha) {
  const RectF dest = sprite.ScaledBounds().Translate(topLeft);
  DrawMaskedSpriteOverlay(sprite, mask, dest, alpha, fadeRange, tint,
                          isInverted, useMaskAlpha);
}

void BaseRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                    glm::vec2 topLeft, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    glm::vec2 scale) {
  const RectF dest =
      sprite.ScaledBounds().Scale(scale, {0.0f, 0.0f}).Translate(topLeft);
  DrawCCMessageBox(sprite, mask, dest, tint, alpha, fadeRange, effectCt);
}

void BaseRenderer::DrawSprite3DRotated(
    const Sprite& sprite, const RectF& dest, const float depth,
    const glm::vec2 vanishingPoint, const bool stayInScreen,
    const glm::quat rot, const glm::vec4 tint, const bool inverted) {
  CornersQuad corners = dest;
  std::array<glm::vec4, 4> vertices = {
      glm::vec4(corners.TopLeft, 0.0f, 1.0f),
      glm::vec4(corners.TopRight, 0.0f, 1.0f),
      glm::vec4(corners.BottomRight, 0.0f, 1.0f),
      glm::vec4(corners.BottomLeft, 0.0f, 1.0f)};

  const glm::mat4 transformation =
      Transformation3D(glm::vec3(0.0f), {dest.Center(), 0.0f}, rot);
  for (glm::vec4& vertex : vertices) vertex = transformation * vertex;

  if (stayInScreen) {
    float maxZ = 0.0f;
    for (const glm::vec4 vertex : vertices) maxZ = std::max(maxZ, vertex.z);

    for (glm::vec4& vertex : vertices) vertex.z -= maxZ;
  }

  for (glm::vec4 vertex : vertices) {
    vertex -= glm::vec4(vanishingPoint, 0.0f, 0.0f);
    vertex *= depth / (depth - vertex.z);
    vertex += glm::vec4(vanishingPoint, 0.0f, 0.0f);
  }

  corners.TopLeft = glm::vec2(vertices[0]);
  corners.TopRight = glm::vec2(vertices[1]);
  corners.BottomRight = glm::vec2(vertices[2]);
  corners.BottomLeft = glm::vec2(vertices[3]);

  DrawSprite(sprite, corners, tint, inverted);
}

void BaseRenderer::DrawSprite3DRotated(Sprite const& sprite, glm::vec2 topLeft,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       glm::vec4 tint, glm::vec2 scale,
                                       bool inverted) {
  const RectF dest =
      sprite.ScaledBounds().Scale(scale, {0.0f, 0.0f}).Translate(topLeft);
  DrawSprite3DRotated(sprite, dest, depth, vanishingPoint, stayInScreen, rot,
                      tint, inverted);
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

void BaseRenderer::DrawCharacterMvl(const Sprite& sprite,
                                    const std::span<const float> vertices,
                                    const std::span<const uint16_t> indices,
                                    glm::mat4 transformation, glm::vec4 tint,
                                    bool inverted) {
  std::vector<float> transformedVertices(vertices.begin(), vertices.end());

  const size_t vertexCount = vertices.size() / 5;
  for (size_t i = 0; i < vertexCount; i++) {
    const glm::vec2 pos =
        transformation *
        glm::vec4(vertices[i * 5], vertices[i * 5 + 1], 0.0f, 1.0f);
    transformedVertices[i * 5] = pos.x;
    transformedVertices[i * 5 + 1] = pos.y;
  }

  DrawCharacterMvl(sprite, transformedVertices, indices, tint, inverted);
}

void BaseRenderer::DrawCharacterMvl(const Sprite& sprite,
                                    std::span<const float> vertices,
                                    std::span<const uint16_t> indices,
                                    const glm::vec2 offset,
                                    const glm::vec4 tint, const bool inverted) {
  std::vector<float> transformedVertices(vertices.begin(), vertices.end());

  const size_t vertexCount = vertices.size() / 5;
  for (size_t i = 0; i < vertexCount; i++) {
    transformedVertices[i * 5] += offset.x;
    transformedVertices[i * 5 + 1] += offset.y;
  }

  DrawCharacterMvl(sprite, transformedVertices, indices, tint, inverted);
}

void BaseRenderer::DrawProcessedText_BasicFont(
    std::span<const ProcessedTextGlyph> text, BasicFont* font, float opacity,
    RendererOutlineMode outlineMode, bool smoothstepGlyphOpacity,
    float outlineOpacity, SpriteSheet* maskedSheet) {
  // cruddy mages outline
  if (outlineMode != RendererOutlineMode::None) {
    for (int i = 0; i < text.size(); i++) {
      glm::vec4 color = RgbIntToFloat(text[i].Colors.OutlineColor);
      color.a = outlineOpacity;
      if (smoothstepGlyphOpacity) {
        color.a *= glm::smoothstep(0.0f, 1.0f, text[i].Opacity);
      } else {
        color.a *= text[i].Opacity;
      }
      Sprite glyph = font->Glyph(text[i].CharId);
      RectF dest = text[i].DestRect;
      switch (outlineMode) {
        case RendererOutlineMode::Full:
          dest.X--;
          dest.Y--;
          if (maskedSheet) {
            Sprite mask;
            mask.Sheet = *maskedSheet;
            mask.Bounds = dest;
            DrawMaskedSpriteOverlay(glyph, mask, dest, color.a * 255, 256,
                                    color, false, i == 0);
          } else {
            DrawSprite(glyph, dest, color);
          }
          dest.X++;
          dest.Y++;
          [[fallthrough]];
        case RendererOutlineMode::BottomRight:
          dest.X++;
          dest.Y++;
          if (maskedSheet) {
            Sprite mask;
            mask.Sheet = *maskedSheet;
            mask.Bounds = dest;
            DrawMaskedSpriteOverlay(glyph, mask, dest, color.a * 255, 256,
                                    color, false, i == 0);
          } else {
            DrawSprite(glyph, dest, color);
          }
          break;
        default:
          break;
      }
    }
  }

  for (int i = 0; i < text.size(); i++) {
    glm::vec4 color = RgbIntToFloat(text[i].Colors.TextColor);
    color.a = opacity;
    if (smoothstepGlyphOpacity) {
      color.a *= glm::smoothstep(0.0f, 1.0f, text[i].Opacity);
    } else {
      color.a *= text[i].Opacity;
    }
    if (maskedSheet) {
      Sprite mask;
      mask.Sheet = *maskedSheet;
      mask.Bounds = text[i].DestRect;
      DrawMaskedSpriteOverlay(font->Glyph(text[i].CharId), mask,
                              text[i].DestRect, color.a * 255, 256, color,
                              false, false);
    } else {
      DrawSprite(font->Glyph(text[i].CharId), text[i].DestRect, color);
    }
  }
}

void BaseRenderer::DrawProcessedText_LBFont(
    std::span<const ProcessedTextGlyph> text, LBFont* font, float opacity,
    RendererOutlineMode outlineMode, bool smoothstepGlyphOpacity,
    float outlineOpacity, SpriteSheet* maskedSheet) {
  if (outlineMode != RendererOutlineMode::None) {
    for (int i = 0; i < text.size(); i++) {
      glm::vec4 color = RgbIntToFloat(text[i].Colors.OutlineColor);
      color.a = outlineOpacity;
      if (smoothstepGlyphOpacity) {
        color.a *= glm::smoothstep(0.0f, 1.0f, text[i].Opacity);
      } else {
        color.a *= text[i].Opacity;
      }

      float scaleX = text[i].DestRect.Height / font->BitmapEmWidth;
      float scaleY = text[i].DestRect.Height / font->BitmapEmHeight;
      RectF outlineDest;

      switch (outlineMode) {
        case RendererOutlineMode::Full:
          outlineDest =
              RectF(text[i].DestRect.X + scaleX * font->OutlineOffset.x,
                    text[i].DestRect.Y + scaleY * font->OutlineOffset.y,
                    scaleX * font->OutlineCellWidth,
                    scaleY * font->OutlineCellHeight);
          break;
        case RendererOutlineMode::BottomRight:
          outlineDest = RectF(
              text[i].DestRect.X + scaleX * (font->OutlineOffset.x * 3 / 4),
              text[i].DestRect.Y + scaleY * (font->OutlineOffset.y * 3 / 4),
              scaleX * (font->OutlineCellWidth + font->OutlineOffset.x / 2),
              scaleY * (font->OutlineCellHeight + font->OutlineOffset.y / 2));
          break;
        default:
          break;
      }
      if (maskedSheet) {
        Sprite mask;
        mask.Sheet = *maskedSheet;
        mask.Bounds = outlineDest;
        DrawMaskedSpriteOverlay(font->OutlineGlyph(text[i].CharId), mask,
                                outlineDest, color.a * 255, 256, color, false,
                                i == 0);
      } else {
        DrawSprite(font->OutlineGlyph(text[i].CharId), outlineDest, color);
      }
    }
  }

  for (int i = 0; i < text.size(); i++) {
    glm::vec4 color = RgbIntToFloat(text[i].Colors.TextColor);
    color.a = opacity;
    if (smoothstepGlyphOpacity) {
      color.a *= glm::smoothstep(0.0f, 1.0f, text[i].Opacity);
    } else {
      color.a *= text[i].Opacity;
    }

    float scaleX = text[i].DestRect.Height / font->BitmapEmWidth;
    float scaleY = text[i].DestRect.Height / font->BitmapEmHeight;

    RectF foregroundDest =
        RectF(text[i].DestRect.X + scaleX * font->ForegroundOffset.x,
              text[i].DestRect.Y + scaleY * font->ForegroundOffset.y,
              scaleX * font->CellWidth, scaleY * font->CellHeight);
    if (maskedSheet) {
      Sprite mask;
      mask.Sheet = *maskedSheet;
      mask.Bounds = foregroundDest;
      DrawMaskedSpriteOverlay(font->Glyph(text[i].CharId), mask, foregroundDest,
                              color.a * 255, 256, color, false, false);
    } else {
      DrawSprite(font->Glyph(text[i].CharId), foregroundDest, color);
    }
  }

  if (maskedSheet) Flush();
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
