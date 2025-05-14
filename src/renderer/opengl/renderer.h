#pragma once

#include "../renderer.h"
#include "window.h"

#include <map>

#include "shader.h"
#include "glc.h"

#include "../../profile/game.h"

namespace Impacto {
namespace OpenGL {

int constexpr NkMaxVertexMemory = 256 * 1024;
int constexpr NkMaxElementMemory = 128 * 1024;

enum Renderer2DMode {
  R2D_None,
  R2D_Sprite,
  R2D_SpriteInverted,
  R2D_YUVFrame,
  R2D_Masked,
  R2D_MaskedNoAlpha,
  R2D_CCMessageBox,
  R2D_CHLCCMenuBackground,
};

class Renderer : public BaseRenderer {
 public:
  void Init() override;
  void Shutdown() override;

#ifndef IMPACTO_DISABLE_IMGUI
  void ImGuiBeginFrame() override;
#endif

  void BeginFrame() override;
  void BeginFrame2D() override;
  void EndFrame() override;

  uint32_t SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                         int height) override;
  int GetSpriteSheetImage(SpriteSheet const& sheet,
                          std::span<uint8_t> outBuffer) override;
  void FreeTexture(uint32_t id) override;
  YUVFrame* CreateYUVFrame(float width, float height) override;

  void DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                  glm::mat4 transformation, std::span<const glm::vec4, 4> tints,
                  bool inverted, bool disableBlend) override;

  void DrawMaskedSprite(const Sprite& sprite, const Sprite& mask,
                        const CornersQuad& spriteDest,
                        const CornersQuad& maskDest, int alpha, int fadeRange,
                        glm::mat4 spriteTransformation,
                        glm::mat4 maskTransformation,
                        std::span<const glm::vec4, 4> tints, bool isInverted,
                        bool isSameTexture) override;

  void DrawMaskedSpriteOverlay(const Sprite& sprite, const Sprite& mask,
                               const CornersQuad& spriteDest,
                               const CornersQuad& maskDest, int alpha,
                               int fadeRange, glm::mat4 spriteTransformation,
                               glm::mat4 maskTransformation,
                               std::span<const glm::vec4, 4> tints,
                               bool isInverted, bool useMaskAlpha) override;

  void DrawVertices(const SpriteSheet& sheet,
                    std::span<const VertexBufferSprites> vertices,
                    std::span<const uint16_t> indices, glm::mat4 transformation,
                    bool inverted) override;

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt) override;

  void DrawCHLCCMenuBackground(const Sprite& sprite, const Sprite& mask,
                               const RectF& dest, float alpha) override;

  void DrawVideoTexture(const YUVFrame& frame, const RectF& dest,
                        glm::vec4 tint, bool alphaVideo) override;

  void CaptureScreencap(Sprite& sprite) override;

  void SetFramebuffer(size_t buffer) override {
    Flush();
    GLC::BindFramebuffer(GL_FRAMEBUFFER, buffer == 0
                                             ? OpenGLWindow->DrawRT
                                             : GLC::Framebuffers[buffer - 1]);
  };

  int GetFramebufferTexture(size_t buffer) override {
    return buffer == 0 ? OpenGLWindow->ReadRenderTexture
                       : GLC::FramebufferTextures[buffer - 1];
  }

  void EnableScissor() override;
  void SetScissorRect(RectF const& rect) override;
  void DisableScissor() override;

 private:
  void EnsureSpaceAvailable(int vertices, int vertexSize, int indices);
  void EnsureTextureBound(GLuint texture);
  void EnsureModeSprite(bool inverted);
  void Flush() override;

  GLWindow* OpenGLWindow;

  GLuint ShaderProgramSprite;
  GLuint ShaderProgramSpriteInverted;
  GLuint ShaderProgramMaskedSprite;
  GLuint ShaderProgramMaskedSpriteNoAlpha;
  GLuint ShaderProgramYUVFrame;
  GLuint ShaderProgramCCMessageBox;
  GLuint ShaderProgramCHLCCMenuBackground;

  GLuint YUVFrameCbLocation;
  GLuint YUVFrameCrLocation;
  GLuint YUVFrameIsAlphaLocation;
  GLuint MaskedIsInvertedLocation;
  GLuint MaskedIsSameTextureLocation;
  GLuint MaskedNoAlphaIsInvertedLocation;

  GLuint VBO;
  GLuint IBO;
  GLuint VAOSprites;

  GLuint Sampler;

  bool Drawing = false;

  static int constexpr VertexBufferSize = 1024 * 1024;
  static int constexpr IndexBufferCount =
      VertexBufferSize / (4 * sizeof(VertexBufferSprites)) * 6;

  GLuint CurrentTexture = 0;
  Renderer2DMode CurrentMode = R2D_None;
  uint8_t VertexBuffer[VertexBufferSize];
  int VertexBufferFill = 0;
  uint16_t IndexBuffer[IndexBufferCount];
  int IndexBufferFill = 0;

  const glm::mat4 Projection =
      glm::ortho(0.0f, Profile::DesignWidth, Profile::DesignHeight, 0.0f);

  // ShaderCompiler compiler
  ShaderCompiler* Shaders;
};

}  // namespace OpenGL
}  // namespace Impacto
