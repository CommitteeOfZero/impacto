#pragma once

#include "../renderer.h"
#include "window.h"

#include <map>

#include "shader.h"
#include "glc.h"
#include "textureunit.h"

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
                    std::optional<const SpriteSheet> mask,
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
  std::unique_ptr<SpriteShader> SpriteShaderProgram;
  std::unique_ptr<SpriteInvertedShader> SpriteInvertedShaderProgram;
  std::unique_ptr<MaskedSpriteShader> MaskedSpriteShaderProgram;
  std::unique_ptr<MaskedSpriteNoAlphaShader> MaskedSpriteNoAlphaShaderProgram;
  std::unique_ptr<YUVFrameShader> YUVFrameShaderProgram;
  std::unique_ptr<CCMessageBoxShader> CCMessageBoxShaderProgram;
  std::unique_ptr<CHLCCMenuBackgroundShader> CHLCCMenuBackgroundShaderProgram;

  const void* CurrentShaderProgram = nullptr;

  template <typename ShaderType, typename UniformsStruct>
  void UseShader(std::unique_ptr<ShaderType>& shader, UniformsStruct uniforms) {
    static_assert(std::is_base_of<Shader<UniformsStruct>, ShaderType>());

    if (CurrentShaderProgram != &shader) {
      Flush();
      CurrentShaderProgram = &shader;
      shader->Bind();
    }

    if (shader->GetUniforms() != uniforms) {
      Flush();
      shader->UploadUniforms(uniforms);
    }
  }

  void Flush() override;

  void InsertVertices(std::span<const VertexBufferSprites> vertices,
                      std::span<const uint16_t> indices);
  void InsertVerticesQuad(CornersQuad pos, GLuint textureLocation,
                          CornersQuad uv, std::span<const glm::vec4, 4> tints,
                          GLuint maskLocation = 0,
                          CornersQuad maskUV = RectF());
  void InsertVerticesQuad(CornersQuad pos, GLuint textureLocation,
                          CornersQuad uv, glm::vec4 tint = glm::vec4(1.0f),
                          GLuint maskLocation = 0,
                          CornersQuad maskUV = RectF()) {
    InsertVerticesQuad(pos, textureLocation, uv,
                       std::array{tint, tint, tint, tint}, maskLocation,
                       maskUV);
  }

  GLWindow* OpenGLWindow;

  GLuint VBO;
  GLuint IBO;
  GLuint VAOSprites;

  bool Drawing = false;

  static constexpr size_t TextureUnitCount = 15;
  static constexpr std::array<GLint, TextureUnitCount> Textures = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  std::array<std::unique_ptr<TextureUnit>, TextureUnitCount> TextureUnits;

  std::vector<TextureUnit> GetTextureLocations(
      std::span<const uint32_t> textureIds);

  static constexpr size_t MaxVertexCount =
      1024 * 1024 / sizeof(VertexBufferSprites);
  static constexpr uint16_t MaxIndexCount =
      std::numeric_limits<uint16_t>::max();

  std::vector<VertexBufferSprites> VertexBuffer;
  std::vector<uint16_t> IndexBuffer;

  uint16_t NextFreeIndex = 0;

  const glm::mat4 Projection =
      glm::ortho(0.0f, Profile::DesignWidth, Profile::DesignHeight, 0.0f);

  // ShaderCompiler compiler
  std::shared_ptr<ShaderCompiler> Shaders;

  bool ScissorEnabled = false;
};

}  // namespace OpenGL
}  // namespace Impacto
