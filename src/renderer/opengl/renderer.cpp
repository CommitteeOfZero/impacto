#include "renderer.h"

#include <array>
#include <numeric>
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

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene = new Scene3D(OpenGLWindow, Shaders);
    Scene->Init();
  }

  // Generate buffers
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &IBO);
  glGenVertexArrays(1, &VAOSprites);

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
  for (size_t i = 0; i < 4; i++) glEnableVertexAttribArray(i);

  // Make 1x1 white pixel for colored rectangles
  Texture rectTexture;
  rectTexture.Load1x1(0xFF, 0xFF, 0xFF, 0xFF);
  SpriteSheet rectSheet(1.0f, 1.0f);
  rectSheet.Texture = rectTexture.Submit();
  RectSprite = Sprite(rectSheet, 0.0f, 0.0f, 1.0f, 1.0f);

  // Set up shaders
  SpriteShaderProgram.emplace(Shaders.Compile("Sprite"));
  SpriteInvertedShaderProgram.emplace(Shaders.Compile("SpriteInverted"));
  MaskedSpriteShaderProgram.emplace(Shaders.Compile("MaskedSprite"));
  MaskedSpriteNoAlphaShaderProgram.emplace(
      Shaders.Compile("MaskedSpriteNoAlpha"));
  YUVFrameShaderProgram.emplace(Shaders.Compile("YUVFrame"));
  CCMessageBoxShaderProgram.emplace(Shaders.Compile("CCMessageBoxSprite"));
  CHLCCMenuBackgroundShaderProgram.emplace(
      Shaders.Compile("CHLCCMenuBackground"));

  glGenSamplers(Samplers.size(), Samplers.data());
  for (size_t i = 0; i < TextureUnitCount; i++) {
    glBindSampler(i, Samplers[i]);

    glSamplerParameteri(Samplers[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(Samplers[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glSamplerParameteri(Samplers[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(Samplers[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glSamplerParameteri(Samplers[i], GL_TEXTURE_MAX_ANISOTROPY, 16);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindSampler(0, Samplers[0]);
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

  glDeleteSamplers(Samplers.size(), Samplers.data());

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

  VertexBuffer.clear();
  IndexBuffer.clear();
  NextFreeIndex = 0;

  glDisable(GL_CULL_FACE);

  glBindSampler(0, Samplers[0]);
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

void Renderer::InsertVertices(
    const std::span<const VertexBufferSprites> vertices,
    const std::span<const uint16_t> indices) {
  if (vertices.empty() || indices.empty()) return;
  assert(indices.size() <= MaxIndexCount);

  if (IndexBuffer.size() + indices.size() > MaxIndexCount) Flush();

  const uint16_t maxIndex = *std::max_element(indices.begin(), indices.end());

  VertexBuffer.insert(VertexBuffer.end(), vertices.begin(), vertices.end());

  const size_t indexOffset = IndexBuffer.size();
  IndexBuffer.reserve(IndexBuffer.size() + indices.size());
  std::transform(
      indices.begin(), indices.end(), std::back_inserter(IndexBuffer),
      [this](uint16_t index) { return index + this->NextFreeIndex; });

  NextFreeIndex += maxIndex + 1;
}

void Renderer::InsertVerticesQuad(const CornersQuad pos, const CornersQuad uv,
                                  const std::span<const glm::vec4, 4> tints,
                                  const CornersQuad maskUV) {
  const std::array<const VertexBufferSprites, 4> vertices = {
      VertexBufferSprites{
          .Position = pos.BottomLeft,
          .UV = uv.BottomLeft,
          .Tint = tints[0],
          .MaskUV = maskUV.BottomLeft,
      },
      VertexBufferSprites{
          .Position = pos.TopLeft,
          .UV = uv.TopLeft,
          .Tint = tints[1],
          .MaskUV = maskUV.TopLeft,
      },
      VertexBufferSprites{
          .Position = pos.TopRight,
          .UV = uv.TopRight,
          .Tint = tints[2],
          .MaskUV = maskUV.TopRight,
      },
      VertexBufferSprites{
          .Position = pos.BottomRight,
          .UV = uv.BottomRight,
          .Tint = tints[3],
          .MaskUV = maskUV.BottomRight,
      },
  };

  const std::array<const uint16_t, 6> indices = {0, 1, 2, 0, 2, 3};

  InsertVertices(vertices, indices);
}

void Renderer::UseTextures(
    const std::span<const std::pair<uint32_t, size_t>> textureUnitPairs) {
  for (const auto [textureId, unitIndex] : textureUnitPairs) {
    if (TextureUnits[unitIndex].TextureId != textureId &&
        TextureUnits[unitIndex].InUse) {
      Flush();
    }
  }

  for (const auto [textureId, unitIndex] : textureUnitPairs) {
    TextureUnit& textureUnit = TextureUnits[unitIndex];

    glBindTextureUnit(unitIndex, textureId);

    textureUnit.TextureId = textureId;
    textureUnit.InUse = true;
  }
}

void Renderer::DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                          const glm::mat4 transformation,
                          const std::span<const glm::vec4, 4> tints,
                          const glm::vec3 colorShift, const bool inverted,
                          const bool disableBlend,
                          const bool textureWrapRepeat) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawSprite() called before BeginFrame()\n");
    return;
  }

  if (sprite.Sheet.IsScreenCap) Flush();

  if (textureWrapRepeat) {
    Flush();
    glBindSampler(0, Samplers[0]);
    glSamplerParameteri(Samplers[0], GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(Samplers[0], GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  if (disableBlend) {
    Flush();
    glDisable(GL_BLEND);
  }

  // Set uniform variables
  if (inverted) {
    SpriteInvertedUniforms uniforms{
        .Projection = Projection,
        .Transformation = transformation,
        .ColorMap = 0,
    };

    UseShader(*SpriteInvertedShaderProgram, uniforms);

  } else {
    SpriteUniforms uniforms{
        .Projection = Projection,
        .Transformation = transformation,
        .ColorMap = 0,
        .ColorShift = colorShift,
    };

    UseShader(*SpriteShaderProgram, uniforms);
  }

  UseTextures(std::array<std::pair<uint32_t, size_t>, 1>{
      std::pair{sprite.Sheet.Texture, 0}});

  // OK, all good, make quad

  CornersQuad uvDest = sprite.NormalizedBounds();
  if (sprite.Sheet.IsScreenCap) uvDest.FlipVertical();
  InsertVerticesQuad(dest, uvDest, tints);

  if (disableBlend) {
    Flush();
    glEnable(GL_BLEND);
  }

  if (textureWrapRepeat) {
    Flush();
    glBindSampler(0, Samplers[0]);
    glSamplerParameteri(Samplers[0], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(Samplers[0], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
}

void Renderer::DrawMaskedSprite(
    const Sprite& sprite, const Sprite& mask, const CornersQuad& spriteDest,
    const CornersQuad& maskDest, int alpha, const int fadeRange,
    glm::mat4 spriteTransformation, glm::mat4 maskTransformation,
    const std::span<const glm::vec4, 4> tints, const bool isInverted,
    const bool isSameTexture) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawMaskedSprite() called before BeginFrame()\n");
    return;
  }

  alpha = std::clamp(alpha, 0, fadeRange + 256);
  const float alphaRange = 256.0f / fadeRange;
  const float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  // Set uniform variables
  MaskedSpriteUniforms uniforms{
      .Projection = Projection,
      .SpriteTransformation = spriteTransformation,
      .MaskTransformation = maskTransformation,
      .ColorMap = 0,
      .Mask = 2,
      .Alpha = {alphaRange, constAlpha},
      .IsInverted = isInverted,
      .IsSameTexture = isSameTexture,
  };

  UseShader(*MaskedSpriteShaderProgram, uniforms);

  UseTextures(std::array<std::pair<uint32_t, size_t>, 2>{
      std::pair{sprite.Sheet.Texture, 0},
      std::pair{mask.Sheet.Texture, 2},
  });

  // OK, all good, make quad

  CornersQuad uvDest = sprite.NormalizedBounds();
  CornersQuad maskUVDest = CornersQuad(maskDest).Scale(
      {1.0f / sprite.Bounds.Width, 1.0f / sprite.Bounds.Height}, {0.0f, 0.0f});
  if (sprite.Sheet.IsScreenCap) uvDest.FlipVertical();
  if (mask.Sheet.IsScreenCap) maskUVDest.FlipVertical();
  InsertVerticesQuad(spriteDest, uvDest, tints, maskUVDest);
}

void Renderer::DrawMaskedSpriteOverlay(
    const Sprite& sprite, const Sprite& mask, const CornersQuad& spriteDest,
    const CornersQuad& maskDest, int alpha, const int fadeRange,
    glm::mat4 spriteTransformation, glm::mat4 maskTransformation,
    const std::span<const glm::vec4, 4> tints, const bool isInverted,
    const bool useMaskAlpha) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawMaskedSpriteOverlay() called before BeginFrame()\n");
    return;
  }

  alpha = std::clamp(alpha, 0, fadeRange + 256);
  const float alphaRange = 256.0f / fadeRange;
  const float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  if (useMaskAlpha) {
    MaskedSpriteUniforms uniforms{
        .Projection = Projection,
        .SpriteTransformation = spriteTransformation,
        .MaskTransformation = maskTransformation,
        .ColorMap = 0,
        .Mask = 2,
        .Alpha = {alphaRange, constAlpha},
        .IsInverted = isInverted,
        .IsSameTexture = false,
    };

    UseShader(*MaskedSpriteShaderProgram, uniforms);

  } else {
    MaskedSpriteNoAlphaUniforms uniforms{
        .Projection = Projection,
        .SpriteTransformation = spriteTransformation,
        .MaskTransformation = maskTransformation,
        .ColorMap = 0,
        .Mask = 2,
        .Alpha = {alphaRange, constAlpha},
        .IsInverted = isInverted,
    };

    UseShader(*MaskedSpriteNoAlphaShaderProgram, uniforms);
  }

  UseTextures(std::array<std::pair<uint32_t, size_t>, 2>{
      std::pair{sprite.Sheet.Texture, 0},
      std::pair{mask.Sheet.Texture, 2},
  });

  // OK, all good, make quad

  CornersQuad normalizedMaskDest = CornersQuad(maskDest).Scale(
      {1.0f / mask.Sheet.DesignWidth, 1.0f / mask.Sheet.DesignHeight},
      {0.0f, 0.0f});
  InsertVerticesQuad(spriteDest, sprite.NormalizedBounds(), tints,
                     normalizedMaskDest);
}

void Renderer::DrawVertices(const SpriteSheet& sheet,
                            const SpriteSheet* const mask,
                            const std::span<const VertexBufferSprites> vertices,
                            const std::span<const uint16_t> indices,
                            glm::mat4 transformation, const bool inverted) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVertices() called before BeginFrame()\n");
    return;
  }

  // Set uniform variables
  if (mask != nullptr) {
    MaskedSpriteNoAlphaUniforms uniforms{
        .Projection = Projection,
        .SpriteTransformation = transformation,
        .MaskTransformation = glm::mat4(1.0f),
        .ColorMap = 0,
        .Mask = 2,
        .Alpha = {1.0f, 0.0f},
        .IsInverted = inverted,
    };

    UseShader(*MaskedSpriteNoAlphaShaderProgram, uniforms);

  } else {
    if (inverted) {
      SpriteInvertedUniforms uniforms{
          .Projection = Projection,
          .Transformation = transformation,
          .ColorMap = 0,
      };

      UseShader(*SpriteInvertedShaderProgram, uniforms);

    } else {
      SpriteUniforms uniforms{
          .Projection = Projection,
          .Transformation = transformation,
          .ColorMap = 0,
      };

      UseShader(*SpriteShaderProgram, uniforms);
    }
  }

  if (mask == nullptr) {
    UseTextures(std::array<std::pair<uint32_t, size_t>, 1>{
        std::pair{sheet.Texture, 0},
    });
  } else {
    UseTextures(std::array<std::pair<uint32_t, size_t>, 2>{
        std::pair{sheet.Texture, 0},
        std::pair{mask->Texture, 2},
    });
  }

  std::vector<VertexBufferSprites> transformedVertices;
  transformedVertices.resize(vertices.size());

  const auto transformVertex = [this, sheet](VertexBufferSprites info) {
    if (sheet.IsScreenCap) info.UV.y = 1.0f - info.UV.y;
    return info;
  };
  std::transform(vertices.begin(), vertices.end(), transformedVertices.begin(),
                 transformVertex);

  InsertVertices(transformedVertices, indices);
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

  CCMessageBoxUniforms uniforms{
      .Projection = Projection,
      .ColorMap = 0,
      .Mask = 2,
      .Alpha = {alphaRange, constAlpha, effectCt, 0.0f},
  };

  UseShader(*CCMessageBoxShaderProgram, uniforms);

  UseTextures(std::array<std::pair<uint32_t, size_t>, 2>{
      std::pair{sprite.Sheet.Texture, 0},
      std::pair{mask.Sheet.Texture, 2},
  });

  // OK, all good, make quad

  CornersQuad uvDest = sprite.NormalizedBounds();
  if (sprite.Sheet.IsScreenCap) uvDest.FlipVertical();
  InsertVerticesQuad(dest, uvDest, tint, mask.NormalizedBounds());
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

  CHLCCMenuBackgroundUniforms uniforms{
      .Projection = Projection,
      .ColorMap = 0,
      .Mask = 2,
      .Alpha = alpha,
  };

  UseShader(*CHLCCMenuBackgroundShaderProgram, uniforms);

  UseTextures(std::array<std::pair<uint32_t, size_t>, 2>{
      std::pair{sprite.Sheet.Texture, 0},
      std::pair{mask.Sheet.Texture, 2},
  });

  // OK, all good, make quad

  CornersQuad uvDest = sprite.NormalizedBounds();
  if (sprite.Sheet.IsScreenCap) uvDest.FlipVertical();
  InsertVerticesQuad(dest, uvDest, glm::vec4(1.0f), mask.NormalizedBounds());
}

void Renderer::Flush() {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->Flush() called before BeginFrame()\n");
    return;
  }
  if (!VertexBuffer.empty() && !IndexBuffer.empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 VertexBuffer.size() * sizeof(VertexBufferSprites),
                 VertexBuffer.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer.size() * sizeof(uint16_t),
                 IndexBuffer.data(), GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, IndexBuffer.size(), GL_UNSIGNED_SHORT, 0);
  }

  VertexBuffer.clear();
  IndexBuffer.clear();
  NextFreeIndex = 0;

  FlushTextures();
}

void Renderer::DrawVideoTexture(const YUVFrame& frame, const RectF& dest,
                                const glm::vec4 tint, const bool alphaVideo) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVideoTexture() called before BeginFrame()\n");
    return;
  }

  UseTextures(std::array<std::pair<uint32_t, size_t>, 3>{
      std::pair{frame.LumaId, 0},
      std::pair{frame.CbId, 1},
      std::pair{frame.CrId, 2},
  });

  YUVFrameUniforms uniforms{
      .Projection = Projection,
      .Luma = 0,
      .Cb = 1,
      .Cr = 2,
      .IsAlpha = alphaVideo,
  };

  UseShader(*YUVFrameShaderProgram, uniforms);

  // OK, all good, make quad

  InsertVerticesQuad(dest, RectF(0.0f, 0.0f, 1.0f, 1.0f), tint);
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
  if (ScissorEnabled) return;

  Flush();
  glEnable(GL_SCISSOR_TEST);
  ScissorEnabled = true;
}

void Renderer::SetScissorRect(RectF const& rect) {
  if (!ScissorEnabled) return;

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
  if (!ScissorEnabled) return;

  Flush();
  glDisable(GL_SCISSOR_TEST);
  ScissorEnabled = false;
}

void Renderer::SetBlendMode(RendererBlendMode blendMode) {
  Flush();

  switch (blendMode) {
    case RendererBlendMode::Normal:
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      return;
    case RendererBlendMode::Additive:
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      return;
  }
}

void Renderer::Clear(glm::vec4 color) {
  Flush();

  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

}  // namespace OpenGL
}  // namespace Impacto
