#include "renderer.h"

#include <array>
#include "shader.h"
#include "../../profile/game.h"
#include "../../game.h"
#include "3d/scene.h"
#include "yuvframe.h"

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui_custom/backends/imgui_impl_opengl3.h>
#endif

namespace Impacto {
namespace OpenGL {

void Renderer::Init() {
  if (IsInit) return;
  ImpLog(LogLevel::Info, LogChannel::Render,
         "Initializing Renderer2D system\n");
  IsInit = true;

  OpenGLWindow = new GLWindow();
  OpenGLWindow->Init();
  Window = (BaseWindow*)OpenGLWindow;

  Shaders = new ShaderCompiler();

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene = new Scene3D(OpenGLWindow, Shaders);
    Scene->Init();
  }

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

  GLC::InitializeFramebuffers();

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
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexBufferSprites),
                        (void*)offsetof(VertexBufferSprites, MaskUV));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  // Make 1x1 white pixel for colored rectangles
  Texture rectTexture;
  rectTexture.Load1x1(0xFF, 0xFF, 0xFF, 0xFF);
  SpriteSheet rectSheet(1.0f, 1.0f);
  rectSheet.Texture = rectTexture.Submit();
  RectSprite = Sprite(rectSheet, 0.0f, 0.0f, 1.0f, 1.0f);

  // Set up sprite shader
  ShaderProgramSprite = Shaders->Compile("Sprite");
  glUseProgram(ShaderProgramSprite);
  glUniform1i(glGetUniformLocation(ShaderProgramSprite, "ColorMap"), 0);
  ShaderProgramSpriteInverted = Shaders->Compile("Sprite_inverted");
  glUniform1i(glGetUniformLocation(ShaderProgramSpriteInverted, "ColorMap"), 0);

  ShaderProgramMaskedSprite = Shaders->Compile("MaskedSprite");
  glUseProgram(ShaderProgramMaskedSprite);
  glUniform1i(glGetUniformLocation(ShaderProgramMaskedSprite, "ColorMap"), 0);
  MaskedIsInvertedLocation =
      glGetUniformLocation(ShaderProgramMaskedSprite, "IsInverted");
  MaskedIsSameTextureLocation =
      glGetUniformLocation(ShaderProgramMaskedSprite, "IsSameTexture");

  ShaderProgramMaskedSpriteNoAlpha = Shaders->Compile("MaskedSpriteNoAlpha");
  glUseProgram(ShaderProgramMaskedSpriteNoAlpha);
  glUniform1i(
      glGetUniformLocation(ShaderProgramMaskedSpriteNoAlpha, "ColorMap"), 0);
  MaskedNoAlphaIsInvertedLocation =
      glGetUniformLocation(ShaderProgramMaskedSpriteNoAlpha, "IsInverted");

  ShaderProgramYUVFrame = Shaders->Compile("YUVFrame");
  glUseProgram(ShaderProgramYUVFrame);
  glUniform1i(glGetUniformLocation(ShaderProgramYUVFrame, "Luma"), 0);
  YUVFrameCbLocation = glGetUniformLocation(ShaderProgramYUVFrame, "Cb");
  YUVFrameCrLocation = glGetUniformLocation(ShaderProgramYUVFrame, "Cr");
  YUVFrameIsAlphaLocation =
      glGetUniformLocation(ShaderProgramYUVFrame, "IsAlpha");

  ShaderProgramCCMessageBox = Shaders->Compile("CCMessageBoxSprite");
  glUseProgram(ShaderProgramCCMessageBox);
  glUniform1i(glGetUniformLocation(ShaderProgramCCMessageBox, "ColorMap"), 0);
  ShaderProgramCHLCCMenuBackground = Shaders->Compile("CHLCCMenuBackground");

  // No-mipmapping sampler
  glGenSamplers(1, &Sampler);

  // Don't wrap textures
  glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glSamplerParameteri(Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glSamplerParameteri(Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glSamplerParameteri(Sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
}

void Renderer::Shutdown() {
  if (!IsInit) return;
  if (VBO) glDeleteBuffers(1, &VBO);
  if (IBO) glDeleteBuffers(1, &IBO);
  if (VAOSprites) glDeleteVertexArrays(1, &VAOSprites);
  if (RectSprite.Sheet.Texture) glDeleteTextures(1, &RectSprite.Sheet.Texture);
  IsInit = false;

  GLC::DeleteFramebuffers(GLC::Framebuffers.size(), GLC::Framebuffers.data());
  glDeleteTextures(GLC::FramebufferTextures.size(),
                   GLC::FramebufferTextures.data());

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene->Shutdown();
  }
}

#ifndef IMPACTO_DISABLE_IMGUI
void Renderer::ImGuiBeginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}
#endif

void Renderer::BeginFrame() {}

void Renderer::BeginFrame2D() {
  if (Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->BeginFrame() called before EndFrame()\n");
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

void Renderer::EndFrame() {
  if (!Drawing) return;
  Flush();
  Drawing = false;

  glBindSampler(0, 0);
}

uint32_t Renderer::SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                                 int height) {
  uint32_t result;
  glGenTextures(1, &result);
  glBindTexture(GL_TEXTURE_2D, result);

  // Anisotropic filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

  // Load in data
  GLuint texFormat;
  switch (format) {
    case TexFmt_RGBA:
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      texFormat = GL_RGBA;
      break;
    case TexFmt_RGB:
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      texFormat = GL_RGB;
      break;
    case TexFmt_U8:
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      texFormat = GL_RED;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height, 0, texFormat,
               GL_UNSIGNED_BYTE, buffer);

  // Build mip chain
  // TODO do this ourselves outside of Submit(), this can easily cause a
  // framedrop
  glGenerateMipmap(GL_TEXTURE_2D);

  return result;
}

int Renderer::GetSpriteSheetImage(SpriteSheet const& sheet,
                                  std::span<uint8_t> outBuffer) {
  const int bufferSize = sheet.DesignWidth * sheet.DesignHeight * 4;
  assert(outBuffer.size() >= bufferSize);
  glBindTexture(GL_TEXTURE_2D, sheet.Texture);

  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         sheet.Texture, 0);
  glReadPixels(0, 0, sheet.DesignWidth, sheet.DesignHeight, GL_RGBA,
               GL_UNSIGNED_BYTE, outBuffer.data());

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &fbo);

  if (sheet.IsScreenCap) {
    auto itr = outBuffer.begin();
    auto revItr = std::make_reverse_iterator(itr + bufferSize);
    while (itr < revItr.base() - static_cast<int>(sheet.DesignWidth * 4)) {
      std::swap_ranges(itr, itr + sheet.DesignWidth * 4,
                       revItr.base() - (sheet.DesignWidth * 4));
      itr += sheet.DesignWidth * 4;
      revItr += sheet.DesignWidth * 4;
    }
  }

  return bufferSize;
}

void Renderer::FreeTexture(uint32_t id) { glDeleteTextures(1, &id); }

YUVFrame* Renderer::CreateYUVFrame(float width, float height) {
  auto frame = new GLYUVFrame();
  frame->Init(width, height);
  return (YUVFrame*)frame;
}

void Renderer::DrawCharacterMvl(const Sprite& sprite,
                                const std::span<const float> mvlVertices,
                                const std::span<const uint16_t> mvlIndices,
                                const glm::vec4 tint, const bool inverted) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawCharacterMvl() called before BeginFrame()\n");
    return;
  }

  // Draw just the character with this since we need to rebind the index buffer
  // anyway...
  Flush();

  // Do we have space for the whole character?
  const size_t vertexCount = mvlVertices.size() / 5;
  EnsureSpaceAvailable(vertexCount, sizeof(VertexBufferSprites),
                       mvlIndices.size());

  // Are we in sprite mode?
  EnsureModeSprite(inverted);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += vertexCount * sizeof(VertexBufferSprites);

  IndexBufferFill += mvlIndices.size();

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mvlIndices.size() * sizeof(uint16_t),
               mvlIndices.data(), GL_STATIC_DRAW);

  for (int i = 0; i < vertexCount; i++) {
    vertices[i].Position =
        DesignToNDC({mvlVertices[i * 5], mvlVertices[i * 5 + 1]});
    vertices[i].UV = {mvlVertices[i * 5 + 3], mvlVertices[i * 5 + 4]};
    vertices[i].Tint = tint;
  }

  // Flush again and bind back our buffer
  Flush();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               IndexBufferCount * sizeof(IndexBuffer[0]), IndexBuffer,
               GL_STATIC_DRAW);
}

void Renderer::DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                          const std::span<const glm::vec4, 4> tints,
                          const bool inverted) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawSprite() called before BeginFrame()\n");
    return;
  }

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  // Are we in sprite mode?
  EnsureModeSprite(inverted);

  if (sprite.Sheet.IsScreenCap) {
    Flush();
  }

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  if (sprite.Sheet.IsScreenCap) {
    QuadSetUVFlipped(sprite.Bounds, sprite.Sheet.GetDimensions(),
                     &vertices[0].UV, sizeof(VertexBufferSprites));
  } else {
    QuadSetUV(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
              sizeof(VertexBufferSprites));
  }
  QuadSetPosition(dest, &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tints[i];
}

void Renderer::DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                                const CornersQuad& dest, int alpha,
                                const int fadeRange,
                                const std::span<const glm::vec4, 4> tints,
                                const bool isInverted,
                                const bool isSameTexture) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawMaskedSprite() called before BeginFrame()\n");
    return;
  }

  alpha = std::clamp(alpha, 0, fadeRange + 256);
  const float alphaRange = 256.0f / fadeRange;
  const float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  if (CurrentMode != R2D_Masked) {
    Flush();
    CurrentMode = R2D_Masked;
  }

  EnsureTextureBound(sprite.Sheet.Texture);

  glBindVertexArray(VAOSprites);
  glUseProgram(ShaderProgramMaskedSprite);
  glUniform1i(glGetUniformLocation(ShaderProgramMaskedSprite, "Mask"), 2);
  glUniform2f(glGetUniformLocation(ShaderProgramMaskedSprite, "Alpha"),
              alphaRange, constAlpha);
  glUniform1i(MaskedIsInvertedLocation, isInverted);
  glUniform1i(MaskedIsSameTextureLocation, isSameTexture);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mask.Sheet.Texture);
  glBindSampler(2, Sampler);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  if (sprite.Sheet.IsScreenCap) {
    QuadSetUVFlipped(sprite.Bounds, sprite.Sheet.GetDimensions(),
                     &vertices[0].UV, sizeof(VertexBufferSprites));
  } else {
    QuadSetUV(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
              sizeof(VertexBufferSprites));
  }
  QuadSetUV(sprite.Bounds, sprite.Bounds.GetSize(), &vertices[0].MaskUV,
            sizeof(VertexBufferSprites));

  QuadSetPosition(dest, &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tints[i];
}

void Renderer::DrawMaskedSpriteOverlay(const Sprite& sprite, const Sprite& mask,
                                       const CornersQuad& dest, int alpha,
                                       const int fadeRange,
                                       const glm::vec4 tint,
                                       const bool isInverted,
                                       const bool useMaskAlpha) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawMaskedSpriteOverlay() called before BeginFrame()\n");
    return;
  }

  alpha = std::clamp(alpha, 0, fadeRange + 256);
  const float alphaRange = 256.0f / fadeRange;
  const float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  glBindVertexArray(VAOSprites);
  if (useMaskAlpha) {
    if (CurrentMode != R2D_Masked || sprite.Sheet.IsScreenCap) {
      Flush();
      CurrentMode = R2D_Masked;
    }
    glUseProgram(ShaderProgramMaskedSprite);
    glUniform1i(glGetUniformLocation(ShaderProgramMaskedSprite, "Mask"), 2);
    glUniform2f(glGetUniformLocation(ShaderProgramMaskedSprite, "Alpha"),
                alphaRange, constAlpha);
    glUniform1i(MaskedIsInvertedLocation, isInverted);
    glUniform1i(MaskedIsSameTextureLocation, false);
  } else {
    if (CurrentMode != R2D_MaskedNoAlpha || sprite.Sheet.IsScreenCap) {
      Flush();
      CurrentMode = R2D_MaskedNoAlpha;
    }
    glUseProgram(ShaderProgramMaskedSpriteNoAlpha);
    glUniform1i(glGetUniformLocation(ShaderProgramMaskedSpriteNoAlpha, "Mask"),
                2);
    glUniform2f(glGetUniformLocation(ShaderProgramMaskedSpriteNoAlpha, "Alpha"),
                alphaRange, constAlpha);
    glUniform1i(MaskedNoAlphaIsInvertedLocation, isInverted);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mask.Sheet.Texture);
  glBindSampler(2, Sampler);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
            sizeof(VertexBufferSprites));

  QuadSetUV(mask.Bounds, mask.Sheet.GetDimensions(), &vertices[0].MaskUV,
            sizeof(VertexBufferSprites));

  QuadSetPosition(dest, &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawVertices(SpriteSheet const& sheet,
                            std::span<const glm::vec2> sheetPositions,
                            std::span<const glm::vec2> displayPositions,
                            int width, int height, glm::vec4 tint,
                            bool inverted, bool disableBlend) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVertices() called before BeginFrame()\n");
    return;
  }

  if (disableBlend) {
    glDisable(GL_BLEND);
  }

  const int verticesCount = sheetPositions.size();

  if (verticesCount != displayPositions.size()) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVertices() called with mismatched vertices count\n");
    return;
  }
  Flush();
  glBindSampler(2, Sampler);
  glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

  EnsureSpaceAvailable(verticesCount, sizeof(VertexBufferSprites),
                       verticesCount * 3);
  EnsureModeSprite(inverted);

  EnsureTextureBound(sheet.Texture);
  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += verticesCount * sizeof(VertexBufferSprites);
  std::vector<uint16_t> indices;
  indices.reserve((width - 1) * (height - 1) * 6);
  // Generate indices for triangles
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
  IndexBufferFill += indices.size();

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferFill * sizeof(uint16_t),
               indices.data(), GL_STATIC_DRAW);

  for (int i = 0; i < verticesCount; i++) {
    vertices[i].Position = DesignToNDC(displayPositions[i]);
    vertices[i].Tint = tint;
    glm::vec2 uv =
        sheetPositions[i] / glm::vec2(sheet.DesignWidth, sheet.DesignHeight);
    if (sheet.IsScreenCap) {
      uv.y = 1.0f - uv.y;
    }
    vertices[i].UV = uv;
  }

  // Flush again and bind back our buffer
  Flush();
  glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (disableBlend) {
    glEnable(GL_BLEND);
  }
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               IndexBufferCount * sizeof(IndexBuffer[0]), IndexBuffer,
               GL_STATIC_DRAW);
}

void Renderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                RectF const& dest, glm::vec4 tint, int alpha,
                                int fadeRange, float effectCt) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawCCMessageBox() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  if (CurrentMode != R2D_CCMessageBox) {
    Flush();
    CurrentMode = R2D_CCMessageBox;
  }
  glBindVertexArray(VAOSprites);
  glUseProgram(ShaderProgramCCMessageBox);
  glUniform1i(glGetUniformLocation(ShaderProgramCCMessageBox, "Mask"), 2);
  glUniform4f(glGetUniformLocation(ShaderProgramCCMessageBox, "Alpha"),
              alphaRange, constAlpha, effectCt, 0.0f);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mask.Sheet.Texture);
  glBindSampler(2, Sampler);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  if (sprite.Sheet.IsScreenCap) {
    QuadSetUVFlipped(sprite.Bounds, sprite.Sheet.GetDimensions(),
                     &vertices[0].UV, sizeof(VertexBufferSprites));
  } else {
    QuadSetUV(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
              sizeof(VertexBufferSprites));
  }
  QuadSetUV(mask.Bounds, mask.Sheet.GetDimensions(), &vertices[0].MaskUV,
            sizeof(VertexBufferSprites));

  QuadSetPosition(dest, &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawCHLCCMenuBackground(const Sprite& sprite, const Sprite& mask,
                                       const RectF& dest, float alpha) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawCHLCCMenuBackground() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0.0f)
    alpha = 0;
  else if (alpha > 1.0f)
    alpha = 1.0f;

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  if (CurrentMode != R2D_CHLCCMenuBackground) {
    Flush();
    CurrentMode = R2D_CHLCCMenuBackground;
  }

  glBindVertexArray(VAOSprites);
  glUseProgram(ShaderProgramCHLCCMenuBackground);
  glUniform1i(glGetUniformLocation(ShaderProgramCCMessageBox, "ColorMap"), 0);
  glUniform1i(glGetUniformLocation(ShaderProgramCCMessageBox, "Mask"), 2);
  glUniform1f(glGetUniformLocation(ShaderProgramCHLCCMenuBackground, "Alpha"),
              alpha);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mask.Sheet.Texture);
  glBindSampler(2, Sampler);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUVFlipped(sprite.Bounds, sprite.Sheet.GetDimensions(), &vertices[0].UV,
                   sizeof(VertexBufferSprites));

  QuadSetUV(mask.Bounds, mask.Sheet.GetDimensions(), &vertices[0].MaskUV,
            sizeof(VertexBufferSprites));

  QuadSetPosition(dest, &vertices[0].Position, sizeof(VertexBufferSprites));
}

void Renderer::EnsureSpaceAvailable(int vertices, int vertexSize, int indices) {
  if (VertexBufferFill + vertices * vertexSize > VertexBufferSize ||
      IndexBufferFill + indices > IndexBufferCount) {
    ImpLogSlow(
        LogLevel::Trace, LogChannel::Render,
        "Renderer->EnsureSpaceAvailable flushing because buffers full at "
        "VertexBufferFill=0x{:08x},IndexBufferFill=0x{:08x}\n",
        VertexBufferFill, IndexBufferFill);
    Flush();
  }
}

void Renderer::EnsureTextureBound(GLuint texture) {
  if (CurrentTexture != texture) {
    ImpLogSlow(LogLevel::Trace, LogChannel::Render,
               "Renderer->EnsureTextureBound flushing because texture {:d} is "
               "not {:d}\n",
               CurrentTexture, texture);
    Flush();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    CurrentTexture = texture;
  }
}

void Renderer::EnsureModeSprite(bool inverted) {
  Renderer2DMode wantedMode = inverted ? R2D_SpriteInverted : R2D_Sprite;
  if (CurrentMode != wantedMode) {
    ImpLogSlow(
        LogLevel::Trace, LogChannel::Render,
        "Renderer2D flushing because mode {:d} is not R2D_Sprite/inverted\n",
        to_underlying(CurrentMode));
    Flush();
    glBindVertexArray(VAOSprites);
    glUseProgram(inverted ? ShaderProgramSpriteInverted : ShaderProgramSprite);
    CurrentMode = wantedMode;
  }
}

void Renderer::Flush() {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->Flush() called before BeginFrame()\n");
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
  CurrentTexture = 0;
}

void Renderer::DrawVideoTexture(const YUVFrame& frame, const RectF& dest,
                                const glm::vec4 tint, const bool alphaVideo) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVideoTexture() called before BeginFrame()\n");
    return;
  }

  // Do we have space for one more sprite quad?
  EnsureSpaceAvailable(4, sizeof(VertexBufferSprites), 6);

  if (CurrentMode != R2D_YUVFrame) {
    Flush();
    CurrentMode = R2D_YUVFrame;
  }
  glBindVertexArray(VAOSprites);
  glUseProgram(ShaderProgramYUVFrame);
  glUniform1i(YUVFrameCbLocation, 2);
  glUniform1i(YUVFrameCrLocation, 4);
  glUniform1i(YUVFrameIsAlphaLocation, alphaVideo);

  // Luma
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, frame.LumaId);

  // Cb
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, frame.CbId);
  glBindSampler(2, Sampler);

  // Cr
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, frame.CrId);
  glBindSampler(4, Sampler);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(RectF(0.0f, 0.0f, frame.Width, frame.Height),
            {frame.Width, frame.Height}, &vertices[0].UV,
            sizeof(VertexBufferSprites));
  QuadSetPosition(dest, &vertices[0].Position, sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::CaptureScreencap(Sprite& sprite) {
  Flush();
  sprite.Sheet.IsScreenCap = true;
  sprite.Sheet.DesignWidth = Window->WindowWidth;
  sprite.Sheet.DesignHeight = Window->WindowHeight;

  Window->SwapRTs();
  int prevTextureBinding;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTextureBinding);
  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, Window->WindowWidth,
                   Window->WindowHeight, 0);
  glBindTexture(GL_TEXTURE_2D, prevTextureBinding);
  Window->SwapRTs();
}

void Renderer::EnableScissor() {
  Flush();
  glEnable(GL_SCISSOR_TEST);
}

void Renderer::SetScissorRect(RectF const& rect) {
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
}

void Renderer::DisableScissor() {
  Flush();
  glDisable(GL_SCISSOR_TEST);
}

}  // namespace OpenGL
}  // namespace Impacto
