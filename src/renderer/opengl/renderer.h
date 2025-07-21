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
                  glm::vec3 colorShift, bool inverted, bool disableBlend,
                  bool textureWrapRepeat) override;

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

  void DrawVertices(const SpriteSheet& sheet, const SpriteSheet* mask,
                    ShaderProgramType shaderType,
                    std::span<const VertexBufferSprites> vertices,
                    std::span<const uint16_t> indices,
                    glm::mat4 spriteTransformation,
                    glm::mat4 maskTransformation, bool inverted) override;

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

  void SetStencilMode(StencilBufferMode mode) override;
  void ClearStencilBuffer() override;

  void SetBlendMode(RendererBlendMode blendMode) override;

  void Clear(glm::vec4 color) override;

 private:
  std::optional<SpriteShader> SpriteShaderProgram;
  std::optional<SpriteInvertedShader> SpriteInvertedShaderProgram;
  std::optional<MaskedSpriteShader> MaskedSpriteShaderProgram;
  std::optional<MaskedSpriteNoAlphaShader> MaskedSpriteNoAlphaShaderProgram;
  std::optional<ColorMaskedSpriteShader> ColorMaskedSpriteShaderProgram;
  std::optional<AdditiveMaskedSpriteShader> AdditiveMaskedSpriteShaderProgram;
  std::optional<ColorBurnMaskedSpriteShader> ColorBurnMaskedSpriteShaderProgram;
  std::optional<ColorDodgeMaskedSpriteShader>
      ColorDodgeMaskedSpriteShaderProgram;
  std::optional<HardLightMaskedSpriteShader> HardLightMaskedSpriteShaderProgram;
  std::optional<LinearBurnMaskedSpriteShader>
      LinearBurnMaskedSpriteShaderProgram;
  std::optional<OverlayMaskedSpriteShader> OverlayMaskedSpriteShaderProgram;
  std::optional<ScreenMaskedSpriteShader> ScreenMaskedSpriteShaderProgram;
  std::optional<SoftLightMaskedSpriteShader> SoftLightMaskedSpriteShaderProgram;
  std::optional<YUVFrameShader> YUVFrameShaderProgram;
  std::optional<CCMessageBoxShader> CCMessageBoxShaderProgram;
  std::optional<CHLCCMenuBackgroundShader> CHLCCMenuBackgroundShaderProgram;

  const void* CurrentShaderProgram = nullptr;

  template <typename ShaderType, typename UniformsStruct>
  void UseShader(ShaderType& shader, UniformsStruct uniforms) {
    static_assert(std::is_base_of<Shader<UniformsStruct>, ShaderType>());

    if (CurrentShaderProgram != &shader) {
      Flush();
      CurrentShaderProgram = &shader;
      shader.Bind();
    }

    if (shader.GetUniforms() != uniforms) {
      Flush();
      shader.UploadUniforms(uniforms);
    }
  }

  void Flush() override;

  void InsertVertices(std::span<const VertexBufferSprites> vertices,
                      std::span<const uint16_t> indices);
  void InsertVerticesQuad(CornersQuad pos, CornersQuad uv,
                          std::span<const glm::vec4, 4> tints,
                          CornersQuad maskUV = RectF());
  void InsertVerticesQuad(CornersQuad pos, CornersQuad uv,
                          glm::vec4 tint = glm::vec4(1.0f),
                          CornersQuad maskUV = RectF()) {
    InsertVerticesQuad(pos, uv, std::array{tint, tint, tint, tint}, maskUV);
  }

  GLWindow* OpenGLWindow;

  GLuint VBO;
  GLuint IBO;
  GLuint VAOSprites;

  bool Drawing = false;

  struct TextureUnit {
    uint32_t TextureId = 0;
    bool InUse = false;
  };
  static constexpr size_t TextureUnitCount = 15;
  std::array<TextureUnit, TextureUnitCount> TextureUnits;

  void UseTextures(
      std::span<const std::pair<uint32_t, size_t>> textureUnitPairs);
  void FlushTextures() {
    std::transform(TextureUnits.begin(), TextureUnits.end(),
                   TextureUnits.begin(), [](TextureUnit unit) {
                     unit.InUse = false;
                     return unit;
                   });
  }

  std::array<GLuint, TextureUnitCount> Samplers;

  static constexpr size_t MaxVertexCount =
      1024 * 1024 / sizeof(VertexBufferSprites);
  static constexpr uint16_t MaxIndexCount =
      std::numeric_limits<uint16_t>::max();

  std::vector<VertexBufferSprites> VertexBuffer;
  std::vector<uint16_t> IndexBuffer;

  uint16_t NextFreeIndex = 0;

  const glm::mat4 Projection =
      glm::ortho(0.0f, Profile::DesignWidth, Profile::DesignHeight, 0.0f,
                 -Profile::DesignWidth, Profile::DesignWidth);

  // ShaderCompiler compiler
  ShaderCompiler Shaders;

  bool ScissorEnabled = false;
};

}  // namespace OpenGL
}  // namespace Impacto
