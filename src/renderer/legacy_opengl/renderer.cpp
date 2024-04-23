#include "renderer.h"

#include <array>
#include "../../profile/game.h"
#include "../../game.h"

#ifdef PLATFORM_DREAMCAST
#include <kos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glkos.h>
#include <GL/glext.h>
#endif

namespace Impacto {
namespace LegacyOpenGL {

void Renderer::Init() {
  if (IsInit) return;
  // Warning for anyone looking into this, everything here is kind of scuffed
  // and basically just enough to get something running, I'll try to improve it
  // and maybe even implement everything in the future - Revo
  ImpLog(LL_Info, LC_Render, "Initializing Insane Renderer2D system\n");
  IsInit = true;

  OpenGLWindow = new LegacyGLWindow();
  OpenGLWindow->Init();
  Window = (BaseWindow*)OpenGLWindow;

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    // Ha ha, yeah, not right now
    // Scene = new Scene3D(OpenGLWindow, Shaders);
    // Scene->Init();
  }

  // Make 1x1 white pixel for colored rectangles
  Texture rectTexture;
  rectTexture.Load1x1(0xFF, 0xFF, 0xFF, 0xFF);
  SpriteSheet rectSheet(1.0f, 1.0f);
  rectSheet.Texture = rectTexture.Submit();
  RectSprite = Sprite(rectSheet, 0.0f, 0.0f, 1.0f, 1.0f);

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
}

void Renderer::Shutdown() {
  IsInit = false;

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    // Scene->Shutdown();
  }
}

void Renderer::BeginFrame() {}

void Renderer::BeginFrame2D() {
  if (Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->BeginFrame() called before EndFrame()\n");
    return;
  }

  Drawing = true;
  CurrentTexture = 0;
  CurrentMode = R2D_None;
  VertexBufferFill = 0;
  IndexBufferFill = 0;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame() {
  if (!Drawing) return;
  Flush();
  Drawing = false;
}

uint32_t Renderer::SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                                 int height, uint32_t bufferSize) {
  unsigned int result;
  glGenTextures(1, &result);
  glBindTexture(GL_TEXTURE_2D, result);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Load in data
  GLuint texFormat;
  switch (format) {
    case TexFmt_RGBA:
      texFormat = GL_RGBA;
      break;
    case TexFmt_RGB:
      texFormat = GL_RGB;
      break;
    case TexFmt_U8:
      texFormat = GL_RED;
      break;
#ifdef PLATFORM_DREAMCAST
    case TexFmt_DC_A:
      texFormat = GL_COMPRESSED_ARGB_4444_VQ_TWID_KOS;
      break;
    case TexFmt_DC:
      texFormat = GL_COMPRESSED_RGB_565_VQ_TWID_KOS;
      break;
#endif
  }
  if (format == TexFmt_DC || format == TexFmt_DC_A) {
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height, 0,
                           bufferSize, buffer);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height, 0, texFormat,
                 GL_UNSIGNED_BYTE, buffer);
  }

  return result;
}

void Renderer::FreeTexture(uint32_t id) {
  unsigned int idd = (unsigned int)id;
  glDeleteTextures(1, &idd);
}

YUVFrame* Renderer::CreateYUVFrame(float width, float height) { return NULL; }

void Renderer::DrawRect(RectF const& dest, glm::vec4 color, float angle) {
  DrawSprite(RectSprite, dest, color, angle);
}

void Renderer::DrawSprite3DRotated(Sprite const& sprite, RectF const& dest,
                                   float depth, glm::vec2 vanishingPoint,
                                   bool stayInScreen, glm::quat rot,
                                   glm::vec4 tint, bool inverted) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawSprite3DRotated() called before BeginFrame()\n");
    return;
  }
}

void Renderer::DrawRect3DRotated(RectF const& dest, float depth,
                                 glm::vec2 vanishingPoint, bool stayInScreen,
                                 glm::quat rot, glm::vec4 color) {
  DrawSprite3DRotated(RectSprite, dest, depth, vanishingPoint, stayInScreen,
                      rot, color);
}

void Renderer::DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                                int verticesCount, float* mvlVertices,
                                int indicesCount, uint16_t* mvlIndices,
                                bool inverted, glm::vec4 tint,
                                glm::vec2 scale) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawCharacterMvl() called before BeginFrame()\n");
    return;
  }
}

void Renderer::DrawSprite(Sprite const& sprite, RectF const& dest,
                          glm::vec4 tint, float angle, bool inverted,
                          bool isScreencap) {
  std::array<glm::vec4, 4> tints = {tint, tint, tint, tint};
  std::array<glm::vec2, 4> destQuad = {
      glm::vec2{dest.X, dest.Y + dest.Height},
      glm::vec2{dest.X, dest.Y},
      glm::vec2{dest.X + dest.Width, dest.Y},
      glm::vec2{dest.X + dest.Width, dest.Y + dest.Height},
  };
  DrawSprite(sprite, destQuad, tints, angle, inverted, isScreencap);
}

void Renderer::DrawSprite(Sprite const& sprite,
                          std::array<glm::vec2, 4> const& dest,
                          const std::array<glm::vec4, 4>& tints, float angle,
                          bool inverted, bool isScreencap) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawSprite() called before BeginFrame()\n");
    return;
  }
  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  // Are we in sprite mode?
  EnsureModeSprite(inverted);

  if (isScreencap) {
    Flush();
  }

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  if (isScreencap) {
    QuadSetUVFlipped(sprite.Bounds, sprite.Sheet.DesignWidth,
                     sprite.Sheet.DesignHeight, (uintptr_t)&vertices[0].UV,
                     sizeof(VertexBufferSprites));
  } else {
    QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth,
              sprite.Sheet.DesignHeight, (uintptr_t)&vertices[0].UV,
              sizeof(VertexBufferSprites));
  }
  QuadSetPosition(dest, angle, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tints[i];
}

void Renderer::DrawSpriteOffset(Sprite const& sprite, glm::vec2 topLeft,
                                glm::vec2 displayOffset, glm::vec4 tint,
                                glm::vec2 scale, float angle, bool inverted) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawSprite() called before BeginFrame()\n");
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

  QuadSetPositionOffset(sprite.Bounds, topLeft, displayOffset, scale, angle,
                        (uintptr_t)&vertices[0].Position,
                        sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                                RectF const& dest, glm::vec4 tint, int alpha,
                                int fadeRange, bool isScreencap,
                                bool isInverted, bool isSameTexture) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawMaskedSprite() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  //  Flush();
  //  CurrentMode = R2D_Masked;
  //  glBindVertexArray(VAOSprites);
  //  glUseProgram(ShaderProgramMaskedSprite);
  //  glUniform1i(glGetUniformLocation(ShaderProgramMaskedSprite, "Mask"), 2);
  //  glUniform2f(glGetUniformLocation(ShaderProgramMaskedSprite, "Alpha"),
  //              alphaRange, constAlpha);
  //  glUniform1i(MaskedIsInvertedLocation, isInverted);
  //  glUniform1i(MaskedIsSameTextureLocation, isSameTexture);
  //
  //  glActiveTexture(GL_TEXTURE0);
  //  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);
  //
  //  glActiveTexture(GL_TEXTURE2);
  //  glBindTexture(GL_TEXTURE_2D, mask.Sheet.Texture);
  //  glBindSampler(2, Sampler);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  if (isScreencap) {
    QuadSetUVFlipped(sprite.Bounds, sprite.Sheet.DesignWidth,
                     sprite.Sheet.DesignHeight, (uintptr_t)&vertices[0].UV,
                     sizeof(VertexBufferSprites));
  } else {
    QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth,
              sprite.Sheet.DesignHeight, (uintptr_t)&vertices[0].UV,
              sizeof(VertexBufferSprites));
  }
  QuadSetUV(sprite.Bounds, sprite.Bounds.Width, sprite.Bounds.Height,
            (uintptr_t)&vertices[0].MaskUV, sizeof(VertexBufferSprites));

  QuadSetPosition(dest, 0.0f, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawCHLCCDelusionOverlay(Sprite const& sprite,
                                        Sprite const& mask, RectF const& dest,
                                        int alpha, int fadeRange, float angle) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawCHLCCDelusionOverlay() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;
}

void Renderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                RectF const& dest, glm::vec4 tint, int alpha,
                                int fadeRange, float effectCt,
                                bool isScreencap) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawCCMessageBox() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;
}

void Renderer::DrawCHLCCMenuBackground(const Sprite& sprite, const Sprite& mask,
                                       const RectF& dest, float alpha) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawCHLCCMenuBackground() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0.0f)
    alpha = 0;
  else if (alpha > 1.0f)
    alpha = 1.0f;
}

inline void Renderer::QuadSetUVFlipped(RectF const& spriteBounds,
                                       float designWidth, float designHeight,
                                       uintptr_t uvs, int stride) {
  float topUV = (spriteBounds.Y / designHeight);
  float leftUV = (spriteBounds.X / designWidth);
  float bottomUV = ((spriteBounds.Y + spriteBounds.Height) / designHeight);
  float rightUV = ((spriteBounds.X + spriteBounds.Width) / designWidth);

  // top-left
  *(glm::vec2*)(uvs + 0 * stride) = glm::vec2(leftUV, topUV);
  // bottom-left
  *(glm::vec2*)(uvs + 1 * stride) = glm::vec2(leftUV, bottomUV);
  // bottom-right
  *(glm::vec2*)(uvs + 2 * stride) = glm::vec2(rightUV, bottomUV);
  // top-right
  *(glm::vec2*)(uvs + 3 * stride) = glm::vec2(rightUV, topUV);
}

inline void Renderer::QuadSetUV(RectF const& spriteBounds, float designWidth,
                                float designHeight, uintptr_t uvs, int stride,
                                float angle) {
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

  // bottom-left
  *(glm::vec2*)(uvs + 0 * stride) = bottomLeft;
  // top-left
  *(glm::vec2*)(uvs + 1 * stride) = topLeft;
  // top-right
  *(glm::vec2*)(uvs + 2 * stride) = topRight;
  // bottom-right
  *(glm::vec2*)(uvs + 3 * stride) = bottomRight;
}

inline void Renderer::QuadSetPositionOffset(RectF const& spriteBounds,
                                            glm::vec2 displayXY,
                                            glm::vec2 displayOffset,
                                            glm::vec2 scale, float angle,
                                            uintptr_t positions, int stride) {
  glm::vec2 bottomLeft =
      glm::vec2(spriteBounds.X, spriteBounds.Y + spriteBounds.Height);
  glm::vec2 topLeft = glm::vec2(spriteBounds.X, spriteBounds.Y);
  glm::vec2 topRight =
      glm::vec2(spriteBounds.X + spriteBounds.Width, spriteBounds.Y);
  glm::vec2 bottomRight = glm::vec2(spriteBounds.X + spriteBounds.Width,
                                    spriteBounds.Y + spriteBounds.Height);

  // reset origin to top left
  bottomLeft -= topLeft;
  topRight -= topLeft;
  bottomRight -= topLeft;
  topLeft -= topLeft;

  // Rotate
  if (angle != 0.0f) {
    glm::mat2 rot = Rotate2D(angle);
    glm::vec2 center = (bottomLeft + topRight) * 0.5f;

    bottomLeft = rot * (bottomLeft - center) + center;
    topLeft = rot * (topLeft - center) + center;
    topRight = rot * (topRight - center) + center;
    bottomRight = rot * (bottomRight - center) + center;
  }

  // Scale
  bottomLeft = bottomLeft * scale;
  topLeft = topLeft * scale;
  topRight = topRight * scale;
  bottomRight = bottomRight * scale;

  // Translate to the desired screen position
  glm::vec2 newPos = displayXY - displayOffset * scale + displayOffset;
  bottomLeft += newPos;
  topLeft += newPos;
  topRight += newPos;
  bottomRight += newPos;

  // Store the transformed positions
  *(glm::vec2*)(positions + 0 * stride) = DesignToNDC(bottomLeft);
  *(glm::vec2*)(positions + 1 * stride) = DesignToNDC(topLeft);
  *(glm::vec2*)(positions + 2 * stride) = DesignToNDC(topRight);
  *(glm::vec2*)(positions + 3 * stride) = DesignToNDC(bottomRight);
}

inline void Renderer::QuadSetPosition(RectF const& transformedQuad, float angle,
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

inline void Renderer::QuadSetPosition(std::array<glm::vec2, 4> const& destQuad,
                                      float angle, uintptr_t positions,
                                      int stride) {
  glm::vec2 bottomLeft = destQuad[0];
  glm::vec2 topLeft = destQuad[1];
  glm::vec2 topRight = destQuad[2];
  glm::vec2 bottomRight = destQuad[3];

  if (angle != 0.0f) {
    glm::vec2 center = (bottomLeft + topRight) * 0.5f;
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

void Renderer::QuadSetPosition3DRotated(RectF const& transformedQuad,
                                        float depth, glm::vec2 vanishingPoint,
                                        bool stayInScreen, glm::quat rot,
                                        uintptr_t positions, int stride) {
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

void Renderer::EnsureSpaceAvailable(int vertices, int vertexSize, int indices) {
  if (VertexBufferFill + vertices * vertexSize > VertexBufferSize ||
      IndexBufferFill + indices > IndexBufferCount) {
    ImpLogSlow(
        LL_Trace, LC_Render,
        "Renderer->EnsureSpaceAvailable flushing because buffers full at "
        "VertexBufferFill=%08X,IndexBufferFill=%08X\n",
        VertexBufferFill, IndexBufferFill);
    Flush();
  }
}

void Renderer::EnsureTextureBound(uint32_t texture) {
  if (CurrentTexture != texture) {
    ImpLogSlow(LL_Debug, LC_Render,
               "Renderer->EnsureTextureBound flushing because texture %d is "
               "not %d\n",
               CurrentTexture, texture);
    Flush();
    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    CurrentTexture = texture;
  }
}

void Renderer::EnsureModeSprite(bool inverted) {
  Renderer2DMode wantedMode = inverted ? R2D_SpriteInverted : R2D_Sprite;
  if (CurrentMode != wantedMode) {
    ImpLogSlow(
        LL_Trace, LC_Render,
        "Renderer2D flushing because mode %d is not R2D_Sprite/inverted\n",
        CurrentMode);
    Flush();
    CurrentMode = wantedMode;
  }
}

void Renderer::Flush() {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->Flush() called before BeginFrame()\n");
    return;
  }

  if (VertexBufferFill > 0 && IndexBufferFill > 0) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    VertexBufferSprites* vertices = (VertexBufferSprites*)VertexBuffer;
    glTexCoordPointer(2, GL_FLOAT, sizeof(VertexBufferSprites),
                      &vertices[0].UV);
    glColorPointer(4, GL_FLOAT, sizeof(VertexBufferSprites), &vertices[0].Tint);
    glVertexPointer(2, GL_FLOAT, sizeof(VertexBufferSprites), VertexBuffer);
    glDrawElements(GL_TRIANGLES, IndexBufferFill, GL_UNSIGNED_SHORT,
                   IndexBuffer);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  IndexBufferFill = 0;
  VertexBufferFill = 0;
  CurrentTexture = 0;
}

void Renderer::DrawVideoTexture(YUVFrame* tex, RectF const& dest,
                                glm::vec4 tint, float angle, bool alphaVideo) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawVideoTexture() called before BeginFrame()\n");
    return;
  }
}

void Renderer::CaptureScreencap(Sprite const& sprite) {
  // Flush();
}

void Renderer::EnableScissor() {
// Doesn't work on Dreamcast for now :(
#ifndef PLATFROM_DREAMCAST
  Flush();
  glEnable(GL_SCISSOR_TEST);
#endif
}

void Renderer::SetScissorRect(RectF const& rect) {
#ifndef PLATFROM_DREAMCAST
  Rect viewport = Window->GetViewport();
  float scale = fmin((float)Window->WindowWidth / Profile::DesignWidth,
                     (float)Window->WindowHeight / Profile::DesignHeight);
  float rectX = rect.X * scale;
  float rectY = rect.Y * scale;
  float rectWidth = rect.Width * scale;
  float rectHeight = rect.Height * scale;

  glScissor((GLint)(rectX),
            (GLint)((viewport.Height - (GLint)(rectY + rectHeight))),
            (GLint)(rectWidth), (GLint)(rectHeight));
#endif
}

void Renderer::DisableScissor() {
#ifndef PLATFROM_DREAMCAST
  Flush();
  glDisable(GL_SCISSOR_TEST);
#endif
}

}  // namespace LegacyOpenGL
}  // namespace Impacto
