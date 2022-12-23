#include "renderer.h"

#include "opengl/renderer.h"

namespace Impacto {

BaseRenderer* Renderer;
BaseWindow* Window;

GraphicsApi GraphicsApiHint;
GraphicsApi ActualGraphicsApi;

void InitRenderer() {
  Renderer = new OpenGL::Renderer();
  Renderer->Init();
}

void BaseRenderer::Init() { InitImpl(); }

void BaseRenderer::Shutdown() { ShutdownImpl(); }

void BaseRenderer::BeginFrame() { BeginFrameImpl(); }

void BaseRenderer::EndFrame() { EndFrameImpl(); }

uint32_t BaseRenderer::SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                                     int height) {
  return SubmitTextureImpl(format, buffer, width, height);
}

void BaseRenderer::FreeTexture(uint32_t id) { FreeTextureImpl(id); }

YUVFrame* BaseRenderer::CreateYUVFrame(int width, int height) {
  return CreateYUVFrameImpl(width, height);
}

void BaseRenderer::DrawSprite(Sprite const& sprite, RectF const& dest,
                              glm::vec4 tint, float angle, bool inverted,
                              bool isScreencap) {
  DrawSpriteImpl(sprite, dest, tint, angle, inverted, isScreencap);
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

void BaseRenderer::DrawProcessedText(ProcessedTextGlyph* text, int length,
                                     Font* font, float opacity, bool outlined,
                                     bool smoothstepGlyphOpacity) {
  DrawProcessedTextImpl(text, length, font, opacity, outlined,
                        smoothstepGlyphOpacity);
}

void BaseRenderer::DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                                    int verticesCount, float* mvlVertices,
                                    int indicesCount, uint16_t* mvlIndices,
                                    bool inverted, glm::vec4 tint) {
  DrawCharacterMvlImpl(sprite, topLeft, verticesCount, mvlVertices,
                       indicesCount, mvlIndices, inverted, tint);
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

}  // namespace Impacto
