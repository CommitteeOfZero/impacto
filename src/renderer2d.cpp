#include "renderer2d.h"

#include "log.h"
#include "shader.h"

namespace Impacto {

static bool IsInit = false;

static GLuint ShaderProgramSprite;

void Renderer2D::Init() {
  if (IsInit) return;
  ImpLog(LL_Info, LC_Render, "Initializing Renderer2D system\n");
  IsInit = true;

  ShaderProgramSprite = ShaderCompile("Sprite");

  glUniform1i(glGetUniformLocation(ShaderProgramSprite, "ColorMap"), 0);
}

Renderer2D::Renderer2D() {
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
}

Renderer2D::~Renderer2D() {
  if (VBO) glDeleteBuffers(1, &VBO);
  if (IBO) glDeleteBuffers(1, &IBO);
  if (VAOSprites) glDeleteVertexArrays(1, &VAOSprites);
}

void Renderer2D::Begin() {
  if (Drawing) {
    ImpLog(LL_Error, LC_Render, "Renderer2D::Begin() called before Finish()\n");
    return;
  }

  Drawing = true;
  CurrentTexture = 0;
  CurrentMode = R2D_None;
  VertexBufferFill = 0;
  IndexBufferFill = 0;
}
void Renderer2D::DrawSprite(Sprite const& sprite, glm::vec2 topLeft,
                            glm::vec4 tint, glm::vec2 scale) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer2D::DrawSprite() called before Begin()\n");
    return;
  }

  // Do we have space for one more sprite quad?
  if (VertexBufferFill + 4 * sizeof(VertexBufferSprites) > VertexBufferSize ||
      IndexBufferFill + 6 > IndexBufferCount) {
    ImpLogSlow(LL_Trace, LC_Render,
               "Renderer2D::DrawSprite flushing because buffers full at "
               "VertexBufferFill=%08X,IndexBufferFill=%08X\n",
               VertexBufferFill, IndexBufferFill);
    Flush();
  }

  // Are we in sprite mode?
  if (CurrentMode != R2D_Sprite) {
    ImpLogSlow(
        LL_Trace, LC_Render,
        "Renderer2D::DrawSprite flushing because mode %d is not R2D_Sprite\n",
        CurrentMode);
    Flush();
    glBindVertexArray(VAOSprites);
    glUseProgram(ShaderProgramSprite);
    CurrentMode = R2D_Sprite;
  }

  // Do we have the texture assigned?
  if (CurrentTexture != sprite.Sheet.Texture) {
    ImpLogSlow(LL_Trace, LC_Render,
               "Renderer2D::DrawSprite flushing because texture %d is not %d\n",
               CurrentTexture, sprite.Sheet.Texture);
    Flush();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);
    CurrentTexture = sprite.Sheet.Texture;
  }

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  RectF scaledDest(topLeft.x, topLeft.y, scale.x * sprite.Bounds.Width,
                   scale.y * sprite.Bounds.Height);

  // Since our Y is upside down, we need to swap the vertices for front face
  // order

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            &vertices[1].UV, &vertices[0].UV, &vertices[3].UV, &vertices[2].UV);
  QuadSetPosition(scaledDest, &vertices[1].Position, &vertices[0].Position,
                  &vertices[3].Position, &vertices[2].Position);

  vertices[1].Tint = vertices[0].Tint = vertices[3].Tint = vertices[2].Tint =
      tint;
}

inline void Renderer2D::QuadSetUV(RectF const& spriteBounds, float designWidth,
                                  float designHeight, glm::vec2* bottomLeft,
                                  glm::vec2* topLeft, glm::vec2* topRight,
                                  glm::vec2* bottomRight) {
  float topUV = (spriteBounds.Y / designHeight);
  float leftUV = (spriteBounds.X / designWidth);
  float bottomUV = ((spriteBounds.Y + spriteBounds.Height) / designHeight);
  float rightUV = ((spriteBounds.X + spriteBounds.Width) / designWidth);

  *bottomLeft = glm::vec2(leftUV, bottomUV);
  *topLeft = glm::vec2(leftUV, topUV);
  *topRight = glm::vec2(rightUV, topUV);
  *bottomRight = glm::vec2(rightUV, bottomUV);
}

inline void Renderer2D::QuadSetPosition(RectF const& transformedQuad,
                                        glm::vec2* bottomLeft,
                                        glm::vec2* topLeft, glm::vec2* topRight,
                                        glm::vec2* bottomRight) {
  RectF quadNDC = DesignToNDC(transformedQuad);
  *bottomLeft = glm::vec2(quadNDC.X, quadNDC.Y - quadNDC.Height);
  *topLeft = glm::vec2(quadNDC.X, quadNDC.Y);
  *topRight = glm::vec2(quadNDC.X + quadNDC.Width, quadNDC.Y);
  *bottomRight =
      glm::vec2(quadNDC.X + quadNDC.Width, quadNDC.Y - quadNDC.Height);
}

void Renderer2D::Finish() {
  if (!Drawing) return;
  Flush();
  Drawing = false;
}

void Renderer2D::Flush() {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render, "Renderer2D::Flush() called before Begin()\n");
    return;
  }
  if (VertexBufferFill == 0 || IndexBufferFill == 0) return;
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // TODO: better to specify the whole thing or just this?
  glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferFill, VertexBuffer);
  glDrawElements(GL_TRIANGLES, IndexBufferFill, GL_UNSIGNED_SHORT, 0);
  IndexBufferFill = 0;
  VertexBufferFill = 0;
}

}  // namespace Impacto