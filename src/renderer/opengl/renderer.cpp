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

  Shaders = std::make_shared<ShaderCompiler>();

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

  // Set up shaders
  SpriteShaderProgram =
      std::make_unique<SpriteShader>(Shaders->Compile("Sprite"));
  SpriteInvertedShaderProgram = std::make_unique<SpriteInvertedShader>(
      Shaders->Compile("SpriteInverted"));
  MaskedSpriteShaderProgram =
      std::make_unique<MaskedSpriteShader>(Shaders->Compile("MaskedSprite"));
  MaskedSpriteNoAlphaShaderProgram =
      std::make_unique<MaskedSpriteNoAlphaShader>(
          Shaders->Compile("MaskedSpriteNoAlpha"));
  YUVFrameShaderProgram =
      std::make_unique<YUVFrameShader>(Shaders->Compile("YUVFrame"));
  CCMessageBoxShaderProgram = std::make_unique<CCMessageBoxShader>(
      Shaders->Compile("CCMessageBoxSprite"));
  CHLCCMenuBackgroundShaderProgram =
      std::make_unique<CHLCCMenuBackgroundShader>(
          Shaders->Compile("CHLCCMenuBackground"));

  // No-mipmapping sampler
  glGenSamplers(1, &Sampler);

  // Don't wrap textures
  glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glSamplerParameteri(Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

  VertexCount = 0;
  IndexCount = 0;
  NextFreeIndex = 0;

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

void Renderer::InsertVertices(
    const std::span<const VertexBufferSprites> vertices,
    const std::span<const uint16_t> indices) {
  if (vertices.empty() || indices.empty()) return;
  assert(indices.size() <= MaxIndexCount);

  if (IndexCount + indices.size() > MaxIndexCount) Flush();

  const uint16_t maxIndex = *std::max_element(indices.begin(), indices.end());

  std::copy(vertices.begin(), vertices.end(),
            VertexBuffer.begin() + VertexCount);
  VertexCount += vertices.size();

  std::transform(
      indices.begin(), indices.end(), IndexBuffer.begin() + IndexCount,
      [this](uint16_t index) { return index + this->NextFreeIndex; });
  IndexCount += indices.size();

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

void Renderer::DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                          const glm::mat4 transformation,
                          const std::span<const glm::vec4, 4> tints,
                          const bool inverted, const bool disableBlend) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawSprite() called before BeginFrame()\n");
    return;
  }

  if (sprite.Sheet.IsScreenCap) Flush();

  // Set uniform variables
  if (inverted) {
    SpriteInvertedUniforms uniforms{
        .Projection = Projection,
        .Transformation = transformation,
        .ColorMap = 0,
    };

    UseShader(SpriteInvertedShaderProgram, uniforms);

  } else {
    SpriteUniforms uniforms{
        .Projection = Projection,
        .Transformation = transformation,
        .ColorMap = 0,
    };

    UseShader(SpriteShaderProgram, uniforms);
  }

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  if (disableBlend) {
    Flush();
    glDisable(GL_BLEND);
  }

  // OK, all good, make quad

  CornersQuad uvDest = sprite.NormalizedBounds();
  if (sprite.Sheet.IsScreenCap) uvDest.FlipVertical();
  InsertVerticesQuad(dest, uvDest, tints);

  if (disableBlend) {
    Flush();
    glEnable(GL_BLEND);
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

  EnsureTextureBound(sprite.Sheet.Texture);

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

  UseShader(MaskedSpriteShaderProgram, uniforms);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mask.Sheet.Texture);
  glBindSampler(2, Sampler);

  // OK, all good, make quad

  CornersQuad uvDest = sprite.NormalizedBounds();
  if (sprite.Sheet.IsScreenCap) uvDest.FlipVertical();
  CornersQuad maskUVDest = CornersQuad(maskDest).Scale(
      {1.0f / sprite.Bounds.Width, 1.0f / sprite.Bounds.Height}, {0.0f, 0.0f});
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

    UseShader(MaskedSpriteShaderProgram, uniforms);

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

    UseShader(MaskedSpriteNoAlphaShaderProgram, uniforms);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mask.Sheet.Texture);
  glBindSampler(2, Sampler);

  // OK, all good, make quad

  InsertVerticesQuad(spriteDest, sprite.NormalizedBounds(), tints,
                     mask.NormalizedBounds());
}

void Renderer::DrawVertices(const SpriteSheet& sheet,
                            const std::span<const VertexBufferSprites> vertices,
                            const std::span<const uint16_t> indices,
                            glm::mat4 transformation, const bool inverted) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVertices() called before BeginFrame()\n");
    return;
  }

  EnsureTextureBound(sheet.Texture);

  // Set uniform variables
  if (inverted) {
    SpriteInvertedUniforms uniforms{
        .Projection = Projection,
        .Transformation = transformation,
        .ColorMap = 0,
    };

    UseShader(SpriteInvertedShaderProgram, uniforms);

  } else {
    SpriteUniforms uniforms{
        .Projection = Projection,
        .Transformation = transformation,
        .ColorMap = 0,
    };

    UseShader(SpriteShaderProgram, uniforms);
  }

  if (sheet.IsScreenCap) {
    std::vector<VertexBufferSprites> flippedVertices;
    flippedVertices.reserve(vertices.size());

    const auto vertexInfoToNDC = [this, sheet](VertexBufferSprites info) {
      if (sheet.IsScreenCap) info.UV.y = 1.0f - info.UV.y;
      return info;
    };
    std::transform(vertices.begin(), vertices.end(), flippedVertices.begin(),
                   vertexInfoToNDC);

    InsertVertices(flippedVertices, indices);
  } else {
    InsertVertices(vertices, indices);
  }
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

  UseShader(CCMessageBoxShaderProgram, uniforms);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mask.Sheet.Texture);
  glBindSampler(2, Sampler);

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

  UseShader(CHLCCMenuBackgroundShaderProgram, uniforms);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sprite.Sheet.Texture);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, mask.Sheet.Texture);
  glBindSampler(2, Sampler);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad

  InsertVerticesQuad(dest, sprite.NormalizedBounds(), glm::vec4(1.0f),
                     mask.NormalizedBounds());
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

void Renderer::Flush() {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->Flush() called before BeginFrame()\n");
    return;
  }
  if (VertexCount != 0 && IndexCount != 0) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, VertexCount * sizeof(VertexBufferSprites),
                 VertexBuffer.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(uint16_t),
                 IndexBuffer.data(), GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_SHORT, 0);
  }

  VertexCount = 0;
  IndexCount = 0;
  NextFreeIndex = 0;

  CurrentTexture = 0;
}

void Renderer::DrawVideoTexture(const YUVFrame& frame, const RectF& dest,
                                const glm::vec4 tint, const bool alphaVideo) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVideoTexture() called before BeginFrame()\n");
    return;
  }

  YUVFrameUniforms uniforms{
      .Projection = Projection,
      .Luma = 0,
      .Cb = 2,
      .Cr = 4,
      .IsAlpha = alphaVideo,
  };

  UseShader(YUVFrameShaderProgram, uniforms);

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
