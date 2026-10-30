#pragma once

#include "../renderer.h"

#include "bgfxhandle.h"
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

  void DrawSprite(const PositionedSprite& spriteInfo,
                  const SpriteConfig& config,
                  const StateConfig& stateConfig) override;

  void DrawInvertedSprite(const PositionedSprite& spriteInfo,
                          const InvertedSpriteConfig& config,
                          const StateConfig& stateConfig) override;

  void DrawMaskedBinarySprite(const PositionedMaskedSprite& spriteInfo,
                              const MaskedBinarySpriteConfig& config,
                              const StateConfig& stateConfig) override;

  void DrawMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                        const MaskedSpriteConfig& config,
                        const StateConfig& stateConfig) override;

  void DrawMaskedSpriteNoAlpha(const PositionedMaskedSprite& spriteInfo,
                               const MaskedSpriteNoAlphaConfig& config,
                               const StateConfig& stateConfig) override;

  void DrawCCMessageBox(const PositionedMaskedSprite& spriteInfo,
                        const CCMessageBoxConfig& config,
                        const StateConfig& stateConfig) override;

  void DrawEdgeDetectedSingleSheetFont(
      const PositionedSprite& spriteInfo,
      const EdgeDetectedSingleSheetFontConfig& config,
      const StateConfig& stateConfig) override;
  void DrawEdgeDetectedSingleSheetFont(
      const PositionedMaskedSprite& spriteInfo,
      const EdgeDetectedSingleSheetFontConfig& config,
      const StateConfig& stateConfig) override;

  void DrawCHLCCMenuBackground(const PositionedMaskedSprite& spriteInfo,
                               const CHLCCMenuBackgroundConfig& config,
                               const StateConfig& stateConfig) override;

  void DrawBlurredSprite(const PositionedSprite& spriteInfo,
                         const BlurredSpriteConfig& config,
                         const StateConfig& stateConfig) override;

  void DrawMosaic(const PositionedSprite& spriteInfo,
                  const MosaicConfig& config,
                  const StateConfig& stateConfig) override;

  void DrawVideoTexture(YUVFrame const& frame, RectF const& dest,
                        glm::vec4 tint, bool alphaVideo) override;
  void DrawVideoTexture(NV12Frame const& frame, RectF const& dest,
                        glm::vec4 tint, bool alphaVideo) override;

  void DrawSilhouette(const PositionedSprite& spriteInfo,
                      const SilhouetteConfig& config,
                      const StateConfig& stateConfig) override;

  void DrawAdditiveMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                                const AdditiveMaskedSpriteConfig& config,
                                const StateConfig& stateConfig) override;

  void DrawColorBurnMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                                 const ColorBurnMaskedSpriteConfig& config,
                                 const StateConfig& stateConfig) override;

  void DrawColorDodgeMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                                  const ColorDodgeMaskedSpriteConfig& config,
                                  const StateConfig& stateConfig) override;

  void DrawColorMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                             const ColorMaskedSpriteConfig& config,
                             const StateConfig& stateConfig) override;

  void DrawHardLightMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                                 const HardLightMaskedSpriteConfig& config,
                                 const StateConfig& stateConfig) override;

  void DrawLinearBurnMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                                  const LinearBurnMaskedSpriteConfig& config,
                                  const StateConfig& stateConfig) override;

  void DrawOverlayMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                               const OverlayMaskedSpriteConfig& config,
                               const StateConfig& stateConfig) override;

  void DrawScreenMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                              const ScreenMaskedSpriteConfig& config,
                              const StateConfig& stateConfig) override;

  void DrawSoftLightMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                                 const SoftLightMaskedSpriteConfig& config,
                                 const StateConfig& stateConfig) override;

  void CaptureScreencap(Sprite& sprite) override {}

  void SetFrameBuffer(size_t frameBufferId) override;
  TextureRef GetFrameBufferTexture(size_t frameBufferId) override;

  void SetScissorRect(std::optional<RectF> rect) override;

  void SetStencilMode(
      std::optional<StateConfig::StencilModeType> mode) override;
  void ClearStencilBuffer() override;

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

  bgfx::ViewId CurrentView = 0;
  void SetRenderView(bgfx::ViewId newRenderView, size_t frameBufferId);
  void InitRenderView(bgfx::ViewId viewId);

  // Only call bgfx::shutdown after all managed objects in this class have been
  // default-destructed
  struct BgfxContextHandleStruct {
    ~BgfxContextHandleStruct() { bgfx::shutdown(); }
  };
  BgfxContextHandleStruct BgfxContextHandle;

  struct RendererState {
    StateConfig GenericState;
    std::reference_wrapper<ShaderProgramInterface> ShaderProgram;
    glm::mat4 Transformation = glm::mat4(1.0f);
  };
  void SetState(RendererState state);
  std::optional<RendererState> CurrentState = std::nullopt;

  struct PersistentRendererState {
    std::optional<RectF> ScissorRect = std::nullopt;
    std::optional<StateConfig::StencilModeType> StencilMode = std::nullopt;

    [[nodiscard]] bool AllDisabled() const;
  };
  PersistentRendererState PersistentState = {};

  constexpr static size_t DrawFrameBufferIdx = 0;
  std::array<FrameBuffer, FrameBufferCount> FrameBuffers;
  size_t CurrentFrameBufferId = std::numeric_limits<size_t>::max();
  size_t RequestedFrameBufferId = std::numeric_limits<size_t>::max();
  std::array<bool, FrameBufferCount> ClearedFrameBuffers;
  glm::vec<2, size_t> Resolution;

  BgfxHandle<bgfx::DynamicIndexBufferHandle> IndexBuffer;
  BgfxHandle<bgfx::DynamicVertexBufferHandle> VertexBuffer;
  std::vector<uint16_t> Indices;
  std::vector<VertexBufferSprites> Vertices;
  size_t CurFrameIndexBufferOffset = 0;
  size_t CurFrameVertexBufferOffset = 0;

  BgfxHandle<bgfx::IndexBufferHandle> BackBufferIndexBuffer;
  BgfxHandle<bgfx::VertexBufferHandle> BackBufferVertexBuffer;

  bgfx::VertexLayout VertexBufferSpritesLayout;

  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::AdditiveMaskedSprite>>
      AdditiveMaskedSpriteShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::CCMessageBoxSprite>>
      CCMessageBoxSpriteShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::CHLCCMenuBackground>>
      CHLCCMenuBackgroundShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::ColorBurnMaskedSprite>>
      ColorBurnMaskedSpriteShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::ColorDodgeMaskedSprite>>
      ColorDodgeMaskedSpriteShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::ColorMaskedSprite>>
      ColorMaskedSpriteShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::EdgeDetectedSingleSheetFont>>
      EdgeDetectedSingleSheetFontShader;
  std::optional<
      ShaderProgram<VertexShaderType::Sprite, FragmentShaderType::GaussianBlur>>
      GaussianBlurShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::HardLightMaskedSprite>>
      HardLightMaskedSpriteShader;
  std::optional<ShaderProgram<VertexShaderType::Sprite,
                              FragmentShaderType::SpriteInverted>>
      InvertedSpriteShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::LinearBurnMaskedSprite>>
      LinearBurnMaskedSpriteShader;
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
      ShaderProgram<VertexShaderType::Sprite, FragmentShaderType::Mosaic>>
      MosaicShader;
  std::optional<
      ShaderProgram<VertexShaderType::Sprite, FragmentShaderType::NV12Frame>>
      NV12FrameShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::OverlayMaskedSprite>>
      OverlayMaskedSpriteShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::ScreenMaskedSprite>>
      ScreenMaskedSpriteShader;
  std::optional<
      ShaderProgram<VertexShaderType::Sprite, FragmentShaderType::Silhouette>>
      SilhouetteShader;
  std::optional<ShaderProgram<VertexShaderType::MaskedSprite,
                              FragmentShaderType::SoftLightMaskedSprite>>
      SoftLightMaskedSpriteShader;
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

  friend class Texture;
};

}  // namespace Impacto::Bgfx
