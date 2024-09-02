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
      ImpLog(LL_Error, LC_Render,
             "Unknown or unsupported renderer selected!\n");
      exit(0);
  }

  Renderer->Init();
}

void BaseRenderer::DrawSprite(Sprite const& sprite, glm::vec2 topLeft,
                              glm::vec4 tint, glm::vec2 scale, float angle,
                              bool inverted, bool isScreencap) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawSprite(sprite, scaledDest, tint, angle, inverted, isScreencap);
}

void BaseRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                    glm::vec2 topLeft, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    bool isScreencap, glm::vec2 scale) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawCCMessageBox(sprite, mask, scaledDest, tint, alpha, fadeRange, effectCt,
                   isScreencap);
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

void BaseRenderer::DrawProcessedText_BasicFont(
    ProcessedTextGlyph* text, int length, BasicFont* font, float opacity,
    RendererOutlineMode outlineMode, bool smoothstepGlyphOpacity,
    float outlineOpacity, RectF bounds) {
  // cruddy mages outline
  if (outlineMode != RendererOutlineMode::RO_None) {
    for (int i = 0; i < length; i++) {
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
        case RendererOutlineMode::RO_Full:
          dest.X--;
          dest.Y--;
          if (bounds.Intersects(dest)) {
            DrawSprite(glyph, dest, color);
          }
          dest.X++;
          dest.Y++;
          [[fallthrough]];
        case RendererOutlineMode::RO_BottomRight:
          dest.X++;
          dest.Y++;
          if (bounds.Intersects(dest)) {
            DrawSprite(glyph, dest, color);
          }
          break;
        default:
          break;
      }
    }
  }

  for (int i = 0; i < length; i++) {
    glm::vec4 color = RgbIntToFloat(text[i].Colors.TextColor);
    color.a = opacity;
    if (smoothstepGlyphOpacity) {
      color.a *= glm::smoothstep(0.0f, 1.0f, text[i].Opacity);
    } else {
      color.a *= text[i].Opacity;
    }
    if (bounds.Intersects(text[i].DestRect)) {
      DrawSprite(font->Glyph(text[i].CharId), text[i].DestRect, color);
    }
  }
}

void BaseRenderer::DrawProcessedText_LBFont(
    ProcessedTextGlyph* text, int length, LBFont* font, float opacity,
    RendererOutlineMode outlineMode, bool smoothstepGlyphOpacity,
    float outlineOpacity, RectF bounds) {
  // TODO: implement outline mode properly
  if (outlineMode != RendererOutlineMode::RO_None) {
    for (int i = 0; i < length; i++) {
      glm::vec4 color = RgbIntToFloat(text[i].Colors.OutlineColor);
      color.a = outlineOpacity;
      if (smoothstepGlyphOpacity) {
        color.a *= glm::smoothstep(0.0f, 1.0f, text[i].Opacity);
      } else {
        color.a *= text[i].Opacity;
      }

      float scale = text[i].DestRect.Height / font->CellHeight;

      RectF outlineDest = RectF(
          text[i].DestRect.X + scale * font->OutlineOffset.x,
          text[i].DestRect.Y + scale * font->OutlineOffset.y,
          scale * font->OutlineCellWidth, scale * font->OutlineCellHeight);

      if (bounds.Intersects(outlineDest)) {
        DrawSprite(font->OutlineGlyph(text[i].CharId), outlineDest, color);
      }
    }
  }

  for (int i = 0; i < length; i++) {
    glm::vec4 color = RgbIntToFloat(text[i].Colors.TextColor);
    color.a = opacity;
    if (smoothstepGlyphOpacity) {
      color.a *= glm::smoothstep(0.0f, 1.0f, text[i].Opacity);
    } else {
      color.a *= text[i].Opacity;
    }
    if (bounds.Intersects(text[i].DestRect)) {
      DrawSprite(font->Glyph(text[i].CharId), text[i].DestRect, color);
    }
  }
}

void BaseRenderer::DrawProcessedText(ProcessedTextGlyph* text, int length,
                                     Font* font, float opacity,
                                     RendererOutlineMode outlineMode,
                                     bool smoothstepGlyphOpacity) {
  DrawProcessedText(text, length, font, opacity, opacity,
                    RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                    outlineMode, smoothstepGlyphOpacity);
}

void BaseRenderer::DrawProcessedText(ProcessedTextGlyph* text, int length,
                                     Font* font, float opacity,
                                     float outlineOpacity,
                                     RendererOutlineMode outlineMode,
                                     bool smoothstepGlyphOpacity) {
  DrawProcessedText(text, length, font, opacity, outlineOpacity,
                    RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                    outlineMode, smoothstepGlyphOpacity);
}

void BaseRenderer::DrawProcessedText(ProcessedTextGlyph* text, int length,
                                     Font* font, float opacity,
                                     float outlineOpacity, RectF bounds,
                                     RendererOutlineMode outlineMode,
                                     bool smoothstepGlyphOpacity) {
  switch (font->Type) {
    case FontType::Basic:
      DrawProcessedText_BasicFont(text, length, (BasicFont*)font, opacity,
                                  outlineMode, smoothstepGlyphOpacity,
                                  outlineOpacity, bounds);
      break;
    case FontType::LB: {
      DrawProcessedText_LBFont(text, length, (LBFont*)font, opacity,
                               outlineMode, smoothstepGlyphOpacity,
                               outlineOpacity, bounds);
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

}  // namespace Impacto
