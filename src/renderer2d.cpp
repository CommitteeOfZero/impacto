#include "renderer2d.h"

#include "log.h"
#include "shader.h"
#include "texture/texture.h"
#include "profile/game.h"

namespace Impacto {
namespace Renderer2D {

static bool IsInit = false;
static GLuint ShaderProgramSprite;
static GLuint ShaderProgramSpriteInverted;

enum Renderer2DMode { R2D_None, R2D_Sprite, R2D_SpriteInverted };

struct VertexBufferSprites {
  glm::vec2 Position;
  glm::vec2 UV;
  glm::vec4 Tint;
};

static int const VertexBufferSize = 32 * 1024;
static int const IndexBufferCount =
    VertexBufferSize / (4 * sizeof(VertexBufferSprites)) * 6;

static void EnsureSpaceAvailable(int vertices, int vertexSize, int indices);
static void EnsureTextureBound(GLuint texture);
static void EnsureModeSprite(bool inverted);
static void Flush();

static inline void QuadSetUV(RectF const& spriteBounds, float designWidth,
                             float designHeight, uintptr_t uvs, int stride);
static inline void QuadSetPosition(RectF const& transformedQuad, float angle,
                                   uintptr_t positions, int stride);
static inline void QuadSetPosition3DRotated(RectF const& transformedQuad,
                                            float depth,
                                            glm::vec2 vanishingPoint,
                                            bool stayInScreen, glm::quat rot,
                                            uintptr_t positions, int stride);

static void DrawProcessedText_BasicFont(ProcessedTextGlyph* text, int length,
                                        BasicFont* font, float opacity,
                                        bool outlined,
                                        bool smoothstepGlyphOpacity);
static void DrawProcessedText_LBFont(ProcessedTextGlyph* text, int length,
                                     LBFont* font, float opacity, bool outlined,
                                     bool smoothstepGlyphOpacity);

static GLuint VBO;
static GLuint IBO;
static GLuint VAOSprites;

static GLuint Sampler;

static bool Drawing = false;

static GLuint CurrentTexture = 0;
static Renderer2DMode CurrentMode = R2D_None;
static uint8_t VertexBuffer[VertexBufferSize];
static int VertexBufferFill = 0;
static uint16_t IndexBuffer[IndexBufferCount];
static int IndexBufferFill = 0;

static Sprite RectSprite;

void Init() {
  if (IsInit) return;
  ImpLog(LL_Info, LC_Render, "Initializing Renderer2D system\n");
  IsInit = true;

  // Fill index buffer with quads
  int index = 0;
  int vertex = 0;
  while (index + 6 <= IndexBufferCount) {
    // bottom-left -> top-left -> top-right
    IndexBuffer[index] = vertex + 0;
    IndexBuffer[index + 1] = vertex + 1;
    IndexBuffer[index + 2] = vertex + 2;
    // bottom-left -> top-right -> bottom-right
    IndexBuffer[index + 3] = vertex + 0;
    IndexBuffer[index + 4] = vertex + 2;
    IndexBuffer[index + 5] = vertex + 3;
    index += 6;
    vertex += 4;
  }

  // Generate buffers
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &IBO);
  glGenVertexArrays(1, &VAOSprites);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, VertexBufferSize, NULL, GL_STREAM_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               IndexBufferCount * sizeof(IndexBuffer[0]), IndexBuffer,
               GL_STATIC_DRAW);

  // Specify vertex layouts
  glBindVertexArray(VAOSprites);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexBufferSprites),
                        (void*)offsetof(VertexBufferSprites, Position));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexBufferSprites),
                        (void*)offsetof(VertexBufferSprites, UV));
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBufferSprites),
                        (void*)offsetof(VertexBufferSprites, Tint));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  // Make 1x1 white pixel for colored rectangles
  Texture rectTexture;
  rectTexture.Load1x1(0xFF, 0xFF, 0xFF, 0xFF);
  SpriteSheet rectSheet(1.0f, 1.0f);
  rectSheet.Texture = rectTexture.Submit();
  RectSprite = Sprite(rectSheet, 0.0f, 0.0f, 1.0f, 1.0f);

  // Set up sprite shader
  ShaderProgramSprite = ShaderCompile("Sprite");
  glUniform1i(glGetUniformLocation(ShaderProgramSprite, "ColorMap"), 0);
  ShaderProgramSpriteInverted = ShaderCompile("Sprite_inverted");
  glUniform1i(glGetUniformLocation(ShaderProgramSpriteInverted, "ColorMap"), 0);

  // No-mipmapping sampler
  glGenSamplers(1, &Sampler);
  glSamplerParameteri(Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glSamplerParameteri(Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glSamplerParameteri(Sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
}

void Shutdown() {
  if (!IsInit) return;
  if (VBO) glDeleteBuffers(1, &VBO);
  if (IBO) glDeleteBuffers(1, &IBO);
  if (VAOSprites) glDeleteVertexArrays(1, &VAOSprites);
  if (RectSprite.Sheet.Texture) glDeleteTextures(1, &RectSprite.Sheet.Texture);
  IsInit = false;
}

void BeginFrame() {
  if (Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer2D::BeginFrame() called before EndFrame()\n");
    return;
  }

  Drawing = true;
  CurrentTexture = 0;
  CurrentMode = R2D_None;
  VertexBufferFill = 0;
  IndexBufferFill = 0;

  glDisable(GL_CULL_FACE);

  // TODO should we really be making this global?
  glBindSampler(0, Sampler);
}

void EndFrame() {
  if (!Drawing) return;
  Flush();
  Drawing = false;

  glBindSampler(0, 0);
}

void DrawSprite(Sprite const& sprite, glm::vec2 topLeft, glm::vec4 tint,
                glm::vec2 scale, float angle, bool inverted) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawSprite(sprite, scaledDest, tint, angle, inverted);
}

void DrawRect(RectF const& dest, glm::vec4 color, float angle) {
  DrawSprite(RectSprite, dest, color, angle);
}

void DrawSprite3DRotated(Sprite const& sprite, RectF const& dest, float depth,
                         glm::vec2 vanishingPoint, bool stayInScreen,
                         glm::quat rot, glm::vec4 tint, bool inverted) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer2D::DrawSprite3DRotated() called before BeginFrame()\n");
    return;
  }

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  // Are we in sprite mode?
  EnsureModeSprite(inverted);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition3DRotated(dest, depth, vanishingPoint, stayInScreen, rot,
                           (uintptr_t)&vertices[0].Position,
                           sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void DrawSprite3DRotated(Sprite const& sprite, glm::vec2 topLeft, float depth,
                         glm::vec2 vanishingPoint, bool stayInScreen,
                         glm::quat rot, glm::vec4 tint, glm::vec2 scale,
                         bool inverted) {
  RectF scaledDest(topLeft.x, topLeft.y,
                   scale.x * sprite.Bounds.Width * sprite.BaseScale.x,
                   scale.y * sprite.Bounds.Height * sprite.BaseScale.y);
  DrawSprite3DRotated(sprite, scaledDest, depth, vanishingPoint, stayInScreen,
                      rot, tint, inverted);
}

void DrawRect3DRotated(RectF const& dest, float depth, glm::vec2 vanishingPoint,
                       bool stayInScreen, glm::quat rot, glm::vec4 color) {
  DrawSprite3DRotated(RectSprite, dest, depth, vanishingPoint, stayInScreen,
                      rot, color);
}

void DrawProcessedText_BasicFont(ProcessedTextGlyph* text, int length,
                                 BasicFont* font, float opacity, bool outlined,
                                 bool smoothstepGlyphOpacity) {
  // cruddy mages outline
  if (outlined) {
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
      dest.X -= 1;
      dest.Y -= 1;
      DrawSprite(glyph, dest, color);
      dest.X += 2;
      dest.Y += 2;
      DrawSprite(glyph, dest, color);
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

void DrawProcessedText_LBFont(ProcessedTextGlyph* text, int length,
                              LBFont* font, float opacity, bool outlined,
                              bool smoothstepGlyphOpacity) {
  if (outlined) {
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

void DrawProcessedText(ProcessedTextGlyph* text, int length, Font* font,
                       float opacity, bool outlined,
                       bool smoothstepGlyphOpacity) {
  switch (font->Type) {
    case FontType::Basic:
      DrawProcessedText_BasicFont(text, length, (BasicFont*)font, opacity,
                                  outlined, smoothstepGlyphOpacity);
      break;
    case FontType::LB: {
      DrawProcessedText_LBFont(text, length, (LBFont*)font, opacity, outlined,
                               smoothstepGlyphOpacity);
    }
  }
}

void DrawSprite(Sprite const& sprite, RectF const& dest, glm::vec4 tint,
                float angle, bool inverted) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer2D::DrawSprite() called before BeginFrame()\n");
    return;
  }

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  // Are we in sprite mode?
  EnsureModeSprite(inverted);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition(dest, angle, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

static inline void QuadSetUV(RectF const& spriteBounds, float designWidth,
                             float designHeight, uintptr_t uvs, int stride) {
  float topUV = (spriteBounds.Y / designHeight);
  float leftUV = (spriteBounds.X / designWidth);
  float bottomUV = ((spriteBounds.Y + spriteBounds.Height) / designHeight);
  float rightUV = ((spriteBounds.X + spriteBounds.Width) / designWidth);

  // bottom-left
  *(glm::vec2*)(uvs + 0 * stride) = glm::vec2(leftUV, bottomUV);
  // top-left
  *(glm::vec2*)(uvs + 1 * stride) = glm::vec2(leftUV, topUV);
  // top-right
  *(glm::vec2*)(uvs + 2 * stride) = glm::vec2(rightUV, topUV);
  // bottom-right
  *(glm::vec2*)(uvs + 3 * stride) = glm::vec2(rightUV, bottomUV);
}

static inline void QuadSetPosition(RectF const& transformedQuad, float angle,
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

  // bottom-left
  *(glm::vec2*)(positions + 0 * stride) = DesignToNDC(bottomLeft);
  // top-left
  *(glm::vec2*)(positions + 1 * stride) = DesignToNDC(topLeft);
  // top-right
  *(glm::vec2*)(positions + 2 * stride) = DesignToNDC(topRight);
  // bottom-right
  *(glm::vec2*)(positions + 3 * stride) = DesignToNDC(bottomRight);
}

void QuadSetPosition3DRotated(RectF const& transformedQuad, float depth,
                              glm::vec2 vanishingPoint, bool stayInScreen,
                              glm::quat rot, uintptr_t positions, int stride) {
  float widthNormalized = transformedQuad.Width / (Profile::DesignWidth * 0.5f);
  float heightNormalized =
      transformedQuad.Height / (Profile::DesignHeight * 0.5f);

  glm::vec4 corners[4]{
      // bottom-left
      {glm::vec2(-widthNormalized / 2.0f, -heightNormalized / 2.0f), 0, 1},
      // top-left
      {glm::vec2(-widthNormalized / 2.0f, heightNormalized / 2.0f), 0, 1},
      // top-right
      {glm::vec2(widthNormalized / 2.0f, heightNormalized / 2.0f), 0, 1},
      // bottom-right
      {glm::vec2(widthNormalized / 2.0f, -heightNormalized / 2.0f), 0, 1}};

  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f),
                     glm::vec3(DesignToNDC(transformedQuad.Center()), 0)) *
      glm::mat4_cast(rot);

  glm::vec4 vanishingPointNDC(DesignToNDC(vanishingPoint), 0, 0);

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

static void EnsureSpaceAvailable(int vertices, int vertexSize, int indices) {
  if (VertexBufferFill + vertices * vertexSize > VertexBufferSize ||
      IndexBufferFill + indices > IndexBufferCount) {
    ImpLogSlow(
        LL_Trace, LC_Render,
        "Renderer2D::EnsureSpaceAvailable flushing because buffers full at "
        "VertexBufferFill=%08X,IndexBufferFill=%08X\n",
        VertexBufferFill, IndexBufferFill);
    Flush();
  }
}

static void EnsureTextureBound(GLuint texture) {
  if (CurrentTexture != texture) {
    ImpLogSlow(LL_Trace, LC_Render,
               "Renderer2D::EnsureTextureBound flushing because texture %d is "
               "not %d\n",
               CurrentTexture, texture);
    Flush();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    CurrentTexture = texture;
  }
}

static void EnsureModeSprite(bool inverted) {
  Renderer2DMode wantedMode = inverted ? R2D_SpriteInverted : R2D_Sprite;
  if (CurrentMode != wantedMode) {
    ImpLogSlow(
        LL_Trace, LC_Render,
        "Renderer2D flushing because mode %d is not R2D_Sprite/inverted\n",
        CurrentMode);
    Flush();
    glBindVertexArray(VAOSprites);
    glUseProgram(inverted ? ShaderProgramSpriteInverted : ShaderProgramSprite);
    CurrentMode = wantedMode;
  }
}

static void Flush() {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer2D::Flush() called before BeginFrame()\n");
    return;
  }
  if (VertexBufferFill > 0 && IndexBufferFill > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // TODO: better to specify the whole thing or just this?
    glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferFill, VertexBuffer);
    glDrawElements(GL_TRIANGLES, IndexBufferFill, GL_UNSIGNED_SHORT, 0);
  }
  IndexBufferFill = 0;
  VertexBufferFill = 0;
}

}  // namespace Renderer2D
}  // namespace Impacto