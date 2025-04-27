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
  CornersQuad dest = CornersQuad(RectF(0.0f, 0.0f, sprite.ScaledWidth(),
                                       sprite.ScaledHeight()))
                         .Transform(transformation);

  DrawSprite(sprite, dest, std::array<glm::vec4, 4>{tint, tint, tint, tint},
             inverted);
}

void BaseRenderer::DrawSprite(Sprite const& sprite, RectF const& dest,
                              glm::vec4 tint, float angle, bool inverted) {
  const glm::mat4 transformation =
      Transformation2D({0.0f, 0.0f}, dest.Center(), angle);
  DrawSprite(sprite, CornersQuad(dest).Transform(transformation),
             std::array<glm::vec4, 4>{tint, tint, tint, tint}, inverted);
}

void BaseRenderer::DrawSprite(Sprite const& sprite, glm::vec2 topLeft,
                              glm::vec4 tint, glm::vec2 scale, float angle,
                              bool inverted) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawSprite(sprite, scaledDest, tint, angle, inverted);
}

void BaseRenderer::DrawSpriteOffset(const Sprite& sprite,
                                    const glm::vec2 topLeft,
                                    const glm::vec2 displayOffset,
                                    const glm::vec4 tint, const glm::vec2 scale,
                                    const float angle, const bool inverted) {
  const glm::mat4 transformation =
      Transformation2D(topLeft, displayOffset, angle, scale);

  DrawSprite(sprite, transformation, tint, inverted);
}

void BaseRenderer::DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                                    const int alpha, const int fadeRange,
                                    const glm::mat4 transformation,
                                    const glm::vec4 tint, const bool isInverted,
                                    const bool isSameTexture) {
  CornersQuad dest = CornersQuad(RectF(0.0f, 0.0f, sprite.ScaledWidth(),
                                       sprite.ScaledHeight()))
                         .Transform(transformation);

  DrawMaskedSprite(sprite, mask, dest, alpha, fadeRange,
                   std::array<glm::vec4, 4>{tint, tint, tint, tint}, isInverted,
                   isSameTexture);
}

void BaseRenderer::DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                                    const RectF& dest, const glm::vec4 tint,
                                    const int alpha, const int fadeRange,
                                    const bool isInverted,
                                    const bool isSameTexture) {
  DrawMaskedSprite(sprite, mask, CornersQuad(dest), alpha, fadeRange,
                   std::array<glm::vec4, 4>{tint, tint, tint, tint}, isInverted,
                   isSameTexture);
}

void BaseRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                    glm::vec2 topLeft, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    glm::vec2 scale) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawCCMessageBox(sprite, mask, scaledDest, tint, alpha, fadeRange, effectCt);
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

  DrawSprite(sprite, corners, std::array<glm::vec4, 4>{tint, tint, tint, tint},
             inverted);
}

void BaseRenderer::DrawSprite3DRotated(Sprite const& sprite, glm::vec2 topLeft,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       glm::vec4 tint, glm::vec2 scale,
                                       bool inverted) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawSprite3DRotated(sprite, scaledDest, depth, vanishingPoint, stayInScreen,
                      rot, tint, inverted);
}

void BaseRenderer::DrawRect(const RectF& dest, const glm::vec4 color,
                            const float angle) {
  DrawSprite(RectSprite, dest, color, angle);
}

void BaseRenderer::DrawRect3DRotated(const RectF& dest, const float depth,
                                     const glm::vec2 vanishingPoint,
                                     const bool stayInScreen,
                                     const glm::quat rot,
                                     const glm::vec4 color) {
  DrawSprite3DRotated(RectSprite, dest, depth, vanishingPoint, stayInScreen,
                      rot, color);
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

void BaseRenderer::DrawVideoTexture(YUVFrame* tex, glm::vec2 topLeft,
                                    glm::vec4 tint, glm::vec2 scale,
                                    float angle, bool alphaVideo) {
  RectF scaledDest(topLeft.x, topLeft.y, scale.x * tex->Width,
                   scale.y * tex->Height);
  DrawVideoTexture(tex, scaledDest, tint, angle, alphaVideo);
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
            DrawMaskedSpriteOverlay(glyph, mask, dest, color, color.a * 255,
                                    256, false, 0, i == 0);
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
            DrawMaskedSpriteOverlay(glyph, mask, dest, color, color.a * 255,
                                    256, false, 0, i == 0);
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
                              text[i].DestRect, color, color.a * 255, 256,
                              false, 0, false);
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
                                outlineDest, color, color.a * 255, 256, false,
                                0, i == 0);
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
                              color, color.a * 255, 256, false, 0, false);
    } else {
      DrawSprite(font->Glyph(text[i].CharId), foregroundDest, color);
    }
  }

  if (maskedSheet) Flush();
}

void BaseRenderer::QuadSetPosition(RectF const& transformedQuad, float angle,
                                   uintptr_t positions, int stride) {
  glm::vec2 bottomLeft =
      glm::vec2(transformedQuad.X, transformedQuad.Y + transformedQuad.Height);
  glm::vec2 topLeft = glm::vec2(transformedQuad.X, transformedQuad.Y);
  glm::vec2 topRight =
      glm::vec2(transformedQuad.X + transformedQuad.Width, transformedQuad.Y);
  glm::vec2 bottomRight = glm::vec2(transformedQuad.X + transformedQuad.Width,
                                    transformedQuad.Y + transformedQuad.Height);

  if (angle != 0.0f) {
    glm::vec2 center = transformedQuad.Center();
    glm::mat2 rot = Rotate2D(angle);

    bottomLeft = rot * (bottomLeft - center) + center;
    topLeft = rot * (topLeft - center) + center;
    topRight = rot * (topRight - center) + center;
    bottomRight = rot * (bottomRight - center) + center;
  }

  // top-left
  *(glm::vec2*)(positions + 0 * stride) = DesignToNDC(topLeft);
  // bottom-left
  *(glm::vec2*)(positions + 1 * stride) = DesignToNDC(bottomLeft);
  // bottom-right
  *(glm::vec2*)(positions + 2 * stride) = DesignToNDC(bottomRight);
  // top-right
  *(glm::vec2*)(positions + 3 * stride) = DesignToNDC(topRight);
}

void BaseRenderer::QuadSetPosition(CornersQuad destQuad, float angle,
                                   uintptr_t positions, int stride) {
  if (angle != 0.0f) {
    glm::vec2 center = (destQuad.BottomLeft + destQuad.TopRight) * 0.5f;
    glm::mat2 rot = Rotate2D(angle);

    destQuad.BottomLeft = rot * (destQuad.BottomLeft - center) + center;
    destQuad.TopLeft = rot * (destQuad.TopLeft - center) + center;
    destQuad.TopRight = rot * (destQuad.TopRight - center) + center;
    destQuad.BottomRight = rot * (destQuad.BottomRight - center) + center;
  }

  // top-left
  *(glm::vec2*)(positions + 0 * stride) = DesignToNDC(destQuad.TopLeft);
  // bottom-left
  *(glm::vec2*)(positions + 1 * stride) = DesignToNDC(destQuad.BottomLeft);
  // bottom-right
  *(glm::vec2*)(positions + 2 * stride) = DesignToNDC(destQuad.BottomRight);
  // top-right
  *(glm::vec2*)(positions + 3 * stride) = DesignToNDC(destQuad.TopRight);
}

void BaseRenderer::QuadSetPositionOffset(
    RectF const& spriteBounds, glm::vec2 displayXY, glm::vec2 displayOffset,
    glm::vec2 scale, float angle, uintptr_t positions, int stride, bool toNDC) {
  glm::vec2 topLeft = {0.0f, 0.0f};
  glm::vec2 bottomLeft = {0.0f, spriteBounds.Height};
  glm::vec2 topRight = {spriteBounds.Width, 0.0f};
  glm::vec2 bottomRight = {spriteBounds.Width, spriteBounds.Height};

  // Translate to origin
  topLeft -= displayOffset;
  bottomLeft -= displayOffset;
  topRight -= displayOffset;
  bottomRight -= displayOffset;

  // Scale
  bottomLeft *= scale;
  topLeft *= scale;
  topRight *= scale;
  bottomRight *= scale;

  // Rotate
  if (angle != 0.0f) {
    const glm::mat2 rot = Rotate2D(angle);

    bottomLeft = rot * bottomLeft;
    topLeft = rot * topLeft;
    topRight = rot * topRight;
    bottomRight = rot * bottomRight;
  }

  // Translate to the desired screen position
  bottomLeft += displayOffset + displayXY;
  topLeft += displayOffset + displayXY;
  topRight += displayOffset + displayXY;
  bottomRight += displayOffset + displayXY;

  if (toNDC) {
    topLeft = DesignToNDC(topLeft);
    bottomLeft = DesignToNDC(bottomLeft);
    bottomRight = DesignToNDC(bottomRight);
    topRight = DesignToNDC(topRight);
  }

  // Store the transformed positions
  *(glm::vec2*)(positions + 0 * stride) = topLeft;
  *(glm::vec2*)(positions + 1 * stride) = bottomLeft;
  *(glm::vec2*)(positions + 2 * stride) = bottomRight;
  *(glm::vec2*)(positions + 3 * stride) = topRight;
}

void BaseRenderer::QuadSetPosition3DRotated(RectF const& transformedQuad,
                                            float depth,
                                            glm::vec2 vanishingPoint,
                                            bool stayInScreen, glm::quat rot,
                                            uintptr_t positions, int stride) {
  float widthNormalized = transformedQuad.Width / (Profile::DesignWidth * 0.5f);
  float heightNormalized =
      transformedQuad.Height / (Profile::DesignHeight * 0.5f);

  glm::vec4 corners[4]{
      // top-left
      {glm::vec2(-widthNormalized / 2.0f, heightNormalized / 2.0f), 0, 1},
      // top-right
      {glm::vec2(widthNormalized / 2.0f, heightNormalized / 2.0f), 0, 1},
      // bottom-right
      {glm::vec2(widthNormalized / 2.0f, -heightNormalized / 2.0f), 0, 1},
      // bottom-left
      {glm::vec2(-widthNormalized / 2.0f, -heightNormalized / 2.0f), 0, 1}};

  glm::mat4 transform =
      glm::translate(
          glm::mat4(1.0f),
          glm::vec3(DesignToNDCNonFlipped(transformedQuad.Center()), 0)) *
      glm::mat4_cast(rot);

  glm::vec4 vanishingPointNDC(DesignToNDCNonFlipped(vanishingPoint), 0, 0);

  for (int i = 0; i < 4; i++) {
    corners[i] = transform * corners[i];
  }

  if (stayInScreen) {
    float maxZ = 0.0f;
    for (int i = 0; i < 4; i++) {
      if (corners[i].z > maxZ) maxZ = corners[i].z;
    }
    for (int i = 0; i < 4; i++) {
      corners[i].z -= maxZ;
    }
  }

  for (int i = 0; i < 4; i++) {
    // perspective
    corners[i] -= vanishingPointNDC;
    corners[i].x *= (depth / (depth - corners[i].z));
    corners[i].y *= (depth / (depth - corners[i].z));
    corners[i] += vanishingPointNDC;

    *(glm::vec2*)(positions + i * stride) = corners[i];
  }
}

void BaseRenderer::QuadSetUV(const RectF spriteBounds, const float designWidth,
                             const float designHeight, glm::vec2* const uvs,
                             const size_t stride, const float angle) {
  float topUV = (spriteBounds.Y / designHeight);
  float leftUV = (spriteBounds.X / designWidth);
  float bottomUV = ((spriteBounds.Y + spriteBounds.Height) / designHeight);
  float rightUV = ((spriteBounds.X + spriteBounds.Width) / designWidth);

  glm::vec2 bottomLeft(leftUV, bottomUV);
  glm::vec2 topLeft(leftUV, topUV);
  glm::vec2 topRight(rightUV, topUV);
  glm::vec2 bottomRight(rightUV, bottomUV);

  if (angle != 0.0f) {
    glm::vec2 center = (bottomLeft + topRight) * 0.5f;  // Center of the quad
    glm::mat2 rot = Rotate2D(angle);

    bottomLeft = rot * (bottomLeft - center) + center;
    topLeft = rot * (topLeft - center) + center;
    topRight = rot * (topRight - center) + center;
    bottomRight = rot * (bottomRight - center) + center;
  }

  // top-left
  *(glm::vec2*)((uint8_t*)uvs + 0 * stride) = topLeft;
  // bottom-left
  *(glm::vec2*)((uint8_t*)uvs + 1 * stride) = bottomLeft;
  // bottom-right
  *(glm::vec2*)((uint8_t*)uvs + 2 * stride) = bottomRight;
  // top-right
  *(glm::vec2*)((uint8_t*)uvs + 3 * stride) = topRight;
}

}  // namespace Impacto
