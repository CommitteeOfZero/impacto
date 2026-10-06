#pragma once

#include "../renderer.h"

#include "framebuffer.h"
#include "shader.h"

#include <magic_enum/magic_enum_containers.hpp>

#include <map>

namespace Impacto::Bgfx {

class Renderer final : public BaseRenderer {
 public:
  Renderer();
  ~Renderer() { Shutdown(); }
  void Init() override;
  void Shutdown() override;

  void UpdateResolution() override;

  [[nodiscard]] RendererType GetType() const override;

#ifndef IMPACTO_DISABLE_IMGUI
  void ImGuiBeginFrame() override;
#endif

  void BeginFrame() override;
  void BeginFrame2D() override;
  void EndFrame() override;

  [[nodiscard]] TextureRef MapSpriteSheet(SpriteSheet const& sheet) override {
    return TextureRef{};
  }
  void UnloadSurf(int surfId) override {}

  [[nodiscard]] TextureRef SubmitTexture(
      TexFmt format, std::span<const uint8_t> buffer,
      glm::vec<2, size_t> dimensions) override;

  [[nodiscard]] int GetSpriteSheetImage(SpriteSheet const& sheet,
                                        std::span<uint8_t> outBuffer) override {
    return 0;
  }

  void DrawSprite(Sprite const& sprite, CornersQuad const& dest,
                  glm::mat4 transformation, std::span<const glm::vec4, 4> tints,
                  glm::vec3 colorShift, bool inverted, bool disableBlend,
                  bool textureWrapRepeat) override;

  void DrawMaskedBinarySprite(const PositionedMaskedSprite& spriteInfo,
                              const MaskedBinarySpriteConfig& config,
                              const StateConfig& stateConfig) override;

  void DrawMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                        const MaskedSpriteConfig& config,
                        const StateConfig& stateConfig) override;

  void DrawMaskedSpriteNoAlpha(const PositionedMaskedSprite& spriteInfo,
                               const MaskedSpriteNoAlphaConfig& config,
                               const StateConfig& stateConfig) override;

  void DrawPrimitives(SpriteSheet const& sheet, SpriteSheet const* mask,
                      ShaderProgramType shaderType,
                      std::span<const VertexBufferSprites> vertices,
                      std::span<const uint16_t> indices,
                      glm::mat4 spriteTransformation,
                      glm::mat4 maskTransformation, bool inverted,
                      TopologyMode topology, bool textureWrapRepeat) override;

  void DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                        RectF const& dest, glm::vec4 tint, int alpha,
                        int fadeRange, float effectCt) override {}

  void DrawEdgeDetectedSingleSheetFont(
      SpriteSheet const& sheet, SpriteSheet const* mask,
      std::span<const VertexBufferSprites> vertices,
      std::span<const uint16_t> indices, float differenceFactor,
      float intensityShift, float alphaShift, glm::vec2 renderScale,
      glm::mat4 spriteTransformation, glm::mat4 maskTransformation) override {}

  void DrawCHLCCMenuBackground(Sprite const& sprite, Sprite const& mask,
                               RectF const& dest, float alpha) override {}

  void DrawBlurredSprite(Sprite const& sprite, CornersQuad const& dest,
                         glm::mat4 transformation,
                         RendererBlurDirection blurDirection,
                         glm::vec4 tint) override {}

  void DrawMosaic(Sprite const& sprite, CornersQuad dest, float tileSize,
                  glm::mat4 transformation, glm::vec4 tint) override {}

  void DrawVideoTexture(YUVFrame const& frame, RectF const& dest,
                        glm::vec4 tint, bool alphaVideo) override;
  void DrawVideoTexture(NV12Frame const& frame, RectF const& dest,
                        glm::vec4 tint, bool alphaVideo) override;

  void DrawSubtitleGlyph(Sprite const& sprite, CornersQuad const& dest,
                         glm::mat4 transformation, glm::vec4 tint) override {}

  void CaptureScreencap(Sprite& sprite) override {}

  void SetFramebuffer(size_t buffer) override {}
  TextureRef GetFramebufferTexture(size_t buffer) override {
    return TextureRef{};
  }

  void EnableScissor() override {}
  void SetScissorRect(RectF const& rect) override {}
  void DisableScissor() override {}

  void SetStencilMode(StateConfig::StencilModeType mode) override {}
  void ClearStencilBuffer() override {}

  void SetBlendMode(StateConfig::BlendModeType blendMode) override {}

  void Clear(glm::vec4 color) override {}

 private:
  void Flush() override;

  void InsertVertices(std::span<const uint16_t> indices,
                      std::span<const VertexBufferSprites> vertices,
                      bool flipVertically);

  void InsertQuad(CornersQuad dest, CornersQuad uvs,
                  std::span<const glm::vec4, 4> tints, bool flipVertically,
                  CornersQuad maskUvs = RectF{});
  void InsertQuad(CornersQuad dest, CornersQuad uvs, glm::vec4 tint,
                  bool flipVertically, CornersQuad maskUvs = RectF{}) {
    InsertQuad(dest, uvs, std::array<glm::vec4, 4>{tint, tint, tint, tint},
               flipVertically, maskUvs);
  }

  bool ShouldFlip(const Texture& texture) const;

  // Only call bgfx::shutdown after all managed objects in this class have been
  // default-destructed
  struct BgfxHandleStruct {
    ~BgfxHandleStruct() { bgfx::shutdown(); }
  };
  BgfxHandleStruct BgfxHandle;

  struct RendererState {
    StateConfig GenericState;
    std::reference_wrapper<ShaderProgramInterface> ShaderProgram;
    glm::mat4 Transformation = glm::mat4(1.0f);
  };
  void SetState(const RendererState& state);
  std::optional<RendererState> CurrentState = std::nullopt;

  FrameBuffer DrawFrameBuffer;
  glm::ivec2 Resolution;

  bgfx::DynamicIndexBufferHandle IndexBuffer = {bgfx::kInvalidHandle};
  bgfx::DynamicVertexBufferHandle VertexBuffer = {bgfx::kInvalidHandle};
  std::vector<uint16_t> Indices;
  std::vector<VertexBufferSprites> Vertices;
  size_t CurFrameIndexBufferOffset = 0;
  size_t CurFrameVertexBufferOffset = 0;

  bgfx::IndexBufferHandle BackBufferIndexBuffer = {bgfx::kInvalidHandle};
  bgfx::VertexBufferHandle BackBufferVertexBuffer = {bgfx::kInvalidHandle};

  bgfx::VertexLayout VertexBufferSpritesLayout;

  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::MaskedSprite>>
      MaskedSpriteShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::MaskedSpriteBinary>>
      MaskedSpriteBinaryShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::MaskedSpriteNoAlpha>>
      MaskedSpriteNoAlphaShader;
  std::optional<
      ShaderProgram<VertexShaderType::Sprite, FragmentShaderType::NV12Frame>>
      NV12FrameShader;
  std::optional<
      ShaderProgram<VertexShaderType::Sprite, FragmentShaderType::Sprite>>
      SpriteShader;
  std::optional<
      ShaderProgram<VertexShaderType::Sprite, FragmentShaderType::YUVFrame>>
      YUVFrameShader;

  std::map<uint64_t, std::pair<std::unique_ptr<Texture>, size_t>> Textures;
  // This vector holds all orphaned texture objects to defer deletion until the
  // start of the next frame
  std::vector<std::unique_ptr<Texture>> OrphanedTextures;

  decltype(Textures)::iterator DeclareTexture(
      std::unique_ptr<Texture>&& texture);

  [[nodiscard]] MutableTextureRef DeclareMutableTexture(
      TexFmt format, glm::vec<2, size_t> dimensions) override;

  void AlterRefCount(TextureInterface* texture, int difference) override;
};

}  // namespace Impacto::Bgfx
