#include "renderer.h"

#include "opengl/renderer.h"

namespace Impacto {

IRenderer* Renderer;

void InitRenderer() {
  auto test = new OpenGL::Renderer();
  test->Init();

  Renderer = test;
}

void IRenderer::Init() { InitImpl(); }

void IRenderer::Shutdown() { ShutdownImpl(); }

void IRenderer::BeginFrame() { BeginFrameImpl(); }

void IRenderer::EndFrame() { EndFrameImpl(); }

void IRenderer::DrawSprite(Sprite const& sprite, RectF const& dest,
                           glm::vec4 tint, float angle, bool inverted,
                           bool isScreencap) {
  DrawSpriteImpl(sprite, dest, tint, angle, inverted, isScreencap);
}

void IRenderer::DrawSprite(Sprite const& sprite, glm::vec2 topLeft,
                           glm::vec4 tint, glm::vec2 scale, float angle,
                           bool inverted, bool isScreencap) {
  DrawSpriteImpl(sprite, topLeft, tint, scale, angle, inverted, isScreencap);
}

void IRenderer::DrawRect(RectF const& dest, glm::vec4 color, float angle) {
  DrawRectImpl(dest, color, angle);
}

void IRenderer::DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                                 RectF const& dest, glm::vec4 tint, int alpha,
                                 int fadeRange, bool isScreencap,
                                 bool isInverted, bool isSameTexture) {
  DrawMaskedSpriteImpl(sprite, mask, dest, tint, alpha, fadeRange, isScreencap,
                       isInverted, isSameTexture);
}

void IRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                 glm::vec2 topLeft, glm::vec4 tint, int alpha,
                                 int fadeRange, float effectCt,
                                 bool isScreencap, glm::vec2 scale) {
  DrawCCMessageBoxImpl(sprite, mask, topLeft, tint, alpha, fadeRange, effectCt,
                       isScreencap, scale);
}

void IRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                 RectF const& dest, glm::vec4 tint, int alpha,
                                 int fadeRange, float effectCt,
                                 bool isScreencap) {
  DrawCCMessageBoxImpl(sprite, mask, dest, tint, alpha, fadeRange, effectCt,
                       isScreencap);
}

void IRenderer::DrawSprite3DRotated(Sprite const& sprite, RectF const& dest,
                                    float depth, glm::vec2 vanishingPoint,
                                    bool stayInScreen, glm::quat rot,
                                    glm::vec4 tint, bool inverted) {
  DrawSprite3DRotatedImpl(sprite, dest, depth, vanishingPoint, stayInScreen,
                          rot, tint, inverted);
}

void IRenderer::DrawSprite3DRotated(Sprite const& sprite, glm::vec2 topLeft,
                                    float depth, glm::vec2 vanishingPoint,
                                    bool stayInScreen, glm::quat rot,
                                    glm::vec4 tint, glm::vec2 scale,
                                    bool inverted) {
  DrawSprite3DRotatedImpl(sprite, topLeft, depth, vanishingPoint, stayInScreen,
                          rot, tint, scale, inverted);
}

void IRenderer::DrawRect3DRotated(RectF const& dest, float depth,
                                  glm::vec2 vanishingPoint, bool stayInScreen,
                                  glm::quat rot, glm::vec4 color) {
  DrawRect3DRotatedImpl(dest, depth, vanishingPoint, stayInScreen, rot, color);
}

void IRenderer::DrawProcessedText(ProcessedTextGlyph* text, int length,
                                  Font* font, float opacity, bool outlined,
                                  bool smoothstepGlyphOpacity) {
  DrawProcessedTextImpl(text, length, font, opacity, outlined,
                        smoothstepGlyphOpacity);
}

void IRenderer::DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                                 int verticesCount, float* mvlVertices,
                                 int indicesCount, uint16_t* mvlIndices,
                                 bool inverted, glm::vec4 tint) {
  DrawCharacterMvlImpl(sprite, topLeft, verticesCount, mvlVertices,
                       indicesCount, mvlIndices, inverted, tint);
}

void IRenderer::DrawVideoTexture(YUVFrame const& tex, RectF const& dest,
                                 glm::vec4 tint, float angle, bool alphaVideo) {
  DrawVideoTextureImpl(tex, dest, tint, angle, alphaVideo);
}

void IRenderer::DrawVideoTexture(YUVFrame const& tex, glm::vec2 topLeft,
                                 glm::vec4 tint, glm::vec2 scale, float angle,
                                 bool alphaVideo) {
  DrawVideoTextureImpl(tex, topLeft, tint, scale, angle, alphaVideo);
}

void IRenderer::CaptureScreencap(Sprite const& sprite) {
  CaptureScreencapImpl(sprite);
}

void IRenderer::EnableScissor() {
  EnableScissorImpl();
}

void IRenderer::SetScissorRect(RectF const& rect) {
  SetScissorRectImpl(rect);
}

void IRenderer::DisableScissor() {
  DisableScissorImpl();
}

}  // namespace Impacto
