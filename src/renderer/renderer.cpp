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

void BaseRenderer::DrawVertices(
    const SpriteSheet& sheet,
    const std::span<const VertexBufferSprites> vertices, const int width,
    const int height, const glm::mat4 transformation, const bool inverted) {
  // Generate indices for triangles
  std::vector<uint16_t> indices;
  indices.reserve((width - 1) * (height - 1) * 6);
  for (int y = 0; y < height - 1; y++) {
    for (int x = 0; x < width - 1; x++) {
      int v0 = y * width + x;
      int v1 = y * width + (x + 1);
      int v2 = (y + 1) * width + x;
      int v3 = (y + 1) * width + (x + 1);

      // First triangle
      for (auto v : {v1, v0, v2}) {
        indices.push_back(v);
      }
      // Second triangle
      for (auto v : {v3, v1, v2}) {
        indices.push_back(v);
      }
    }
  }

  DrawVertices(sheet, vertices, indices, transformation, inverted);
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
                                    glm::mat4(1.0f), color, false, i == 0);
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
                                    glm::mat4(1.0f), color, false, i == 0);
          } else {
            DrawSprite(glyph, dest, color);
          }
          break;

        default:
          ImpLogSlow(LogLevel::Warning, LogChannel::Render,
                     "Unexpected outline mode!");
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
                              text[i].DestRect, color.a * 255, 256,
                              glm::mat4(1.0f), color, false, false);
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
          ImpLogSlow(LogLevel::Warning, LogChannel::Render,
                     "Unexpected outline mode!");
          break;
      }

      if (maskedSheet) {
        Sprite mask;
        mask.Sheet = *maskedSheet;
        mask.Bounds = outlineDest;
        DrawMaskedSpriteOverlay(font->OutlineGlyph(text[i].CharId), mask,
                                outlineDest, color.a * 255, 256,
                                glm::mat4(1.0f), color, false, i == 0);
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
                              color.a * 255, 256, glm::mat4(1.0f), color, false,
                              false);
    } else {
      DrawSprite(font->Glyph(text[i].CharId), foregroundDest, color);
    }
  }

  if (maskedSheet) Flush();
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
