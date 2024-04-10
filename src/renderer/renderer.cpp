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

BaseRenderer* Renderer;
BaseWindow* Window;

GraphicsApi GraphicsApiHint;
GraphicsApi ActualGraphicsApi;

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

void BaseRenderer::Init() { InitImpl(); }

void BaseRenderer::Shutdown() { ShutdownImpl(); }

#ifndef IMPACTO_DISABLE_IMGUI
void BaseRenderer::ImGuiBeginFrame() { ImGuiBeginFrameImpl(); }
#endif

void BaseRenderer::BeginFrame() { BeginFrameImpl(); }

void BaseRenderer::BeginFrame2D() { BeginFrame2DImpl(); }

void BaseRenderer::EndFrame() { EndFrameImpl(); }

uint32_t BaseRenderer::SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                                     int height) {
  return SubmitTextureImpl(format, buffer, width, height);
}

void BaseRenderer::FreeTexture(uint32_t id) { FreeTextureImpl(id); }

YUVFrame* BaseRenderer::CreateYUVFrame(float width, float height) {
  return CreateYUVFrameImpl(width, height);
}

void BaseRenderer::DrawSprite(Sprite const& sprite, RectF const& dest,
                              glm::vec4 tint, float angle, bool inverted,
                              bool isScreencap) {
  DrawSpriteImpl(sprite, dest, tint, angle, inverted, isScreencap);
}

void BaseRenderer::DrawSprite(Sprite const& sprite,
                              std::array<glm::vec2, 4> const& dest,
                              const std::array<glm::vec4, 4>& tints,
                              float angle, bool inverted, bool isScreencap) {
  DrawSpriteImpl(sprite, dest, tints, angle, inverted, isScreencap);
}

void BaseRenderer::DrawSpriteOffset(Sprite const& sprite, glm::vec2 topLeft,
                                    glm::vec2 displayOffset, glm::vec4 tint,
                                    glm::vec2 scale, float angle,
                                    bool inverted) {
  DrawSpriteOffsetImpl(sprite, topLeft, displayOffset, tint, scale, angle,
                       inverted);
}

void BaseRenderer::DrawSprite(Sprite const& sprite, glm::vec2 topLeft,
                              glm::vec4 tint, glm::vec2 scale, float angle,
                              bool inverted, bool isScreencap) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawSpriteImpl(sprite, scaledDest, tint, angle, inverted, isScreencap);
}

void BaseRenderer::DrawRect(RectF const& dest, glm::vec4 color, float angle) {
  DrawRectImpl(dest, color, angle);
}

void BaseRenderer::DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, glm::vec4 tint,
                                    int alpha, int fadeRange, bool isScreencap,
                                    bool isInverted, bool isSameTexture) {
  DrawMaskedSpriteImpl(sprite, mask, dest, tint, alpha, fadeRange, isScreencap,
                       isInverted, isSameTexture);
}

void BaseRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                    glm::vec2 topLeft, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    bool isScreencap, glm::vec2 scale) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawCCMessageBoxImpl(sprite, mask, scaledDest, tint, alpha, fadeRange,
                       effectCt, isScreencap);
}

void BaseRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    bool isScreencap) {
  DrawCCMessageBoxImpl(sprite, mask, dest, tint, alpha, fadeRange, effectCt,
                       isScreencap);
}

void BaseRenderer::DrawSprite3DRotated(Sprite const& sprite, RectF const& dest,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       glm::vec4 tint, bool inverted) {
  DrawSprite3DRotatedImpl(sprite, dest, depth, vanishingPoint, stayInScreen,
                          rot, tint, inverted);
}

void BaseRenderer::DrawSprite3DRotated(Sprite const& sprite, glm::vec2 topLeft,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       glm::vec4 tint, glm::vec2 scale,
                                       bool inverted) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawSprite3DRotatedImpl(sprite, scaledDest, depth, vanishingPoint,
                          stayInScreen, rot, tint, inverted);
}

void BaseRenderer::DrawRect3DRotated(RectF const& dest, float depth,
                                     glm::vec2 vanishingPoint,
                                     bool stayInScreen, glm::quat rot,
                                     glm::vec4 color) {
  DrawRect3DRotatedImpl(dest, depth, vanishingPoint, stayInScreen, rot, color);
}

void BaseRenderer::DrawProcessedText_BasicFont(ProcessedTextGlyph* text,
                                               int length, BasicFont* font,
                                               float opacity,
                                               RendererOutlineMode outlineMode,
                                               bool smoothstepGlyphOpacity) {
  // cruddy mages outline
  if (outlineMode) {
    for (int i = 0; i < length; i++) {
      glm::vec4 color = RgbIntToFloat(text[i].Colors.OutlineColor);
      color.a = opacity;
      if (smoothstepGlyphOpacity) {
        color.a *= glm::smoothstep(0.0f, 1.0f, text[i].Opacity);
      } else {
        color.a *= text[i].Opacity;
      }
      Sprite glyph = font->Glyph(text[i].CharId);
      RectF dest = text[i].DestRect;
      switch (outlineMode) {
        case RO_Full:
          dest.X--;
          dest.Y--;
          DrawSprite(glyph, dest, color);
          dest.X++;
          dest.Y++;
          /*
          TODO: Replace with a fallthrough annoted via [[fallthrough]] once
          we have C++17. Until then, there seems to be no way to silence the
          implicit fallthrough warning in a way compatible with all compilers.
          */
          dest.X++;
          dest.Y++;
          DrawSprite(glyph, dest, color);
          break;
        case RO_BottomRight:
          dest.X++;
          dest.Y++;
          DrawSprite(glyph, dest, color);
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
    DrawSprite(font->Glyph(text[i].CharId), text[i].DestRect, color);
  }
}

void BaseRenderer::DrawProcessedText_LBFont(ProcessedTextGlyph* text,
                                            int length, LBFont* font,
                                            float opacity,
                                            RendererOutlineMode outlineMode,
                                            bool smoothstepGlyphOpacity) {
  // TODO: implement outline mode properly
  if (outlineMode) {
    for (int i = 0; i < length; i++) {
      glm::vec4 color = RgbIntToFloat(text[i].Colors.OutlineColor);
      color.a = opacity;
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

      DrawSprite(font->OutlineGlyph(text[i].CharId), outlineDest, color);
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

    DrawSprite(font->Glyph(text[i].CharId), text[i].DestRect, color);
  }
}

void BaseRenderer::DrawProcessedText(ProcessedTextGlyph* text, int length,
                                     Font* font, float opacity,
                                     RendererOutlineMode outlineMode,
                                     bool smoothstepGlyphOpacity) {
  switch (font->Type) {
    case FontType::Basic:
      DrawProcessedText_BasicFont(text, length, (BasicFont*)font, opacity,
                                  outlineMode, smoothstepGlyphOpacity);
      break;
    case FontType::LB: {
      DrawProcessedText_LBFont(text, length, (LBFont*)font, opacity,
                               outlineMode, smoothstepGlyphOpacity);
    }
  }
}

void BaseRenderer::DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                                    int verticesCount, float* mvlVertices,
                                    int indicesCount, uint16_t* mvlIndices,
                                    bool inverted, glm::vec4 tint,
                                    glm::vec2 scale) {
  DrawCharacterMvlImpl(sprite, topLeft, verticesCount, mvlVertices,
                       indicesCount, mvlIndices, inverted, tint, scale);
}

void BaseRenderer::DrawVideoTexture(YUVFrame* tex, RectF const& dest,
                                    glm::vec4 tint, float angle,
                                    bool alphaVideo) {
  DrawVideoTextureImpl(tex, dest, tint, angle, alphaVideo);
}

void BaseRenderer::DrawVideoTexture(YUVFrame* tex, glm::vec2 topLeft,
                                    glm::vec4 tint, glm::vec2 scale,
                                    float angle, bool alphaVideo) {
  RectF scaledDest(topLeft.x, topLeft.y, scale.x * tex->Width,
                   scale.y * tex->Height);
  DrawVideoTextureImpl(tex, scaledDest, tint, angle, alphaVideo);
}

void BaseRenderer::CaptureScreencap(Sprite const& sprite) {
  CaptureScreencapImpl(sprite);
}

void BaseRenderer::EnableScissor() { EnableScissorImpl(); }

void BaseRenderer::SetScissorRect(RectF const& rect) {
  SetScissorRectImpl(rect);
}

void BaseRenderer::DisableScissor() { DisableScissorImpl(); }

void BaseRenderer::DrawCHLCCMenuBackground(const Sprite& sprite,
                                           const Sprite& mask,
                                           const RectF& dest, float alpha) {
  DrawCHLCCMenuBackgroundImpl(sprite, mask, dest, alpha);
}

void BaseRenderer::DrawCHLCCDelusionOverlay(Sprite const& sprite,
                                            Sprite const& mask,
                                            RectF const& dest, int alpha,
                                            int fadeRange, float angle) {
  DrawCHLCCDelusionOverlayImpl(sprite, mask, dest, alpha, fadeRange, angle);
}

}  // namespace Impacto
