#pragma once

#include "spritesheet.h"
#include "text.h"
#include "video/yuvframe.h"

namespace Impacto {

namespace Renderer2D {

void Init();
void Shutdown();

void BeginFrame();
void EndFrame();

void DrawSprite(Sprite const& sprite, RectF const& dest,
                glm::vec4 tint = glm::vec4(1.0), float angle = 0.0f,
                bool inverted = false, bool isScreencap = false);
void DrawSprite(Sprite const& sprite, glm::vec2 topLeft,
                glm::vec4 tint = glm::vec4(1.0),
                glm::vec2 scale = glm::vec2(1.0), float angle = 0.0f,
                bool inverted = false, bool isScreencap = false);
void DrawRect(RectF const& dest, glm::vec4 color, float angle = 0.0f);

void DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                      RectF const& dest, glm::vec4 tint, int alpha,
                      int fadeRange, bool isScreencap = false,
                      bool isInverted = false, bool isSameTexture = false);

void DrawThing(Sprite const& sprite, RectF const& dest, int alpha,
               int fadeRange, glm::vec4 color);

void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                      glm::vec2 topLeft, glm::vec4 tint, int alpha,
                      int fadeRange, float effectCt, bool isScreencap = false,
                      glm::vec2 scale = glm::vec2(1.0));
void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                      RectF const& dest, glm::vec4 tint, int alpha,
                      int fadeRange, float effectCt, bool isScreencap = false);

void DrawSprite3DRotated(Sprite const& sprite, RectF const& dest, float depth,
                         glm::vec2 vanishingPoint, bool stayInScreen,
                         glm::quat rot, glm::vec4 tint = glm::vec4(1.0f),
                         bool inverted = false);
void DrawSprite3DRotated(Sprite const& sprite, glm::vec2 topLeft, float depth,
                         glm::vec2 vanishingPoint, bool stayInScreen,
                         glm::quat rot, glm::vec4 tint = glm::vec4(1.0f),
                         glm::vec2 scale = glm::vec2(1.0f),
                         bool inverted = false);
void DrawRect3DRotated(RectF const& dest, float depth, glm::vec2 vanishingPoint,
                       bool stayInScreen, glm::quat rot, glm::vec4 color);

void DrawProcessedText(ProcessedTextGlyph* text, int length, Font* font,
                       float opacity = 1.0f, bool outlined = false,
                       bool smoothstepGlyphOpacity = true);

void DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                      int verticesCount, float* mvlVertices, int indicesCount,
                      uint16_t* mvlIndices, bool inverted = false,
                      glm::vec4 tint = glm::vec4(1.0));

void DrawVideoTexture(YUVFrame const& tex, RectF const& dest,
                      glm::vec4 tint = glm::vec4(1.0), float angle = 0.0f,
                      bool alphaVideo = false);
void DrawVideoTexture(YUVFrame const& tex, glm::vec2 topLeft,
                      glm::vec4 tint = glm::vec4(1.0),
                      glm::vec2 scale = glm::vec2(1.0), float angle = 0.0f,
                      bool alphaVideo = false);

void CaptureScreencap(Sprite const& sprite);

void EnableScissor();
void SetScissorRect(RectF const& rect);
void DisableScissor();

}  // namespace Renderer2D
}  // namespace Impacto