#pragma once

#include "window.h"
#include "positionedsprite.h"

#include "3d/scene.h"
#include "../text/text.h"

#include "video/yuvframe.h"
#include "video/nv12frame.h"

enum class RendererType : int {
#ifdef IMPACTO_RENDERER_OPENGL
  OpenGL,
#endif
#ifdef IMPACTO_RENDERER_OPENGLES
  OpenGLES,
#endif
#ifdef IMPACTO_RENDERER_VULKAN
  Vulkan,
#endif
#ifdef IMPACTO_RENDERER_DIRECT3D11
  Direct3D11,
#endif
#ifdef IMPACTO_RENDERER_DIRECT3D12
  Direct3D12,
#endif
#ifdef IMPACTO_RENDERER_METAL
  Metal,
#endif
};

constexpr inline RendererType DefaultRendererType =
#if defined(SDL_PLATFORM_LINUX)
    RendererType::Vulkan;
#elif defined(SDL_PLATFORM_WINDOWS)
    RendererType::Direct3D12;
#elif defined(SDL_PLATFORM_APPLE)
    RendererType::Metal;
#elif defined(SDL_PLATFORM_ANDROID) || defined(__SWITCH__)
    RendererType::OpenGLES;
#else
    RendererType{};
static_assert(false && "No default renderer supplied for target renderer");
#endif

namespace Impacto {

inline GraphicsApi GraphicsApiHint;
inline GraphicsApi ActualGraphicsApi;

constexpr inline size_t FrameBufferCount = 11;

enum class ShaderProgramType : uint8_t {
  AdditiveMaskedSprite,
  ColorBurnMaskedSprite,
  ColorDodgeMaskedSprite,
  ColorMaskedSprite,
  HardLightMaskedSprite,
  LinearBurnMaskedSprite,
  MaskedSpriteNoAlpha,
  OverlayMaskedSprite,
  ScreenMaskedSprite,
  SoftLightMaskedSprite,
  Sprite
};

enum class RendererBlurDirection { Horizontal, Vertical };

// Workaround for compiler bugs gah (ノ｀Д´)ノ
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88165
// https://github.com/llvm/llvm-project/issues/36032
template <typename T>
static inline T CreateDefault() {
  return {};
}

class BaseRenderer {
 public:
  virtual ~BaseRenderer() = default;
  virtual void Init() = 0;
  virtual void Shutdown() = 0;

  virtual void UpdateResolution() = 0;

  virtual RendererType GetType() const = 0;

#ifndef IMPACTO_DISABLE_IMGUI
  virtual void ImGuiBeginFrame() = 0;
#endif

  virtual void BeginFrame() = 0;
  virtual void BeginFrame2D() = 0;
  virtual void EndFrame() = 0;

  inline static ankerl::unordered_dense::map<
      Io::AssetPath, uint32_t, Io::AssetPathHash, Io::AssetPathEqual>
      SheetPathToId;
  inline static ankerl::unordered_dense::map<uint32_t, uint32_t>
      LookupTextureIdToTexture;
  inline static ankerl::unordered_dense::map<int, Io::AssetPathKey> SurfToId;

  [[nodiscard]] virtual TextureRef MapSpriteSheet(SpriteSheet const& sheet) = 0;
  virtual void UnloadSurf(int surfId) = 0;

  [[nodiscard]] virtual TextureRef SubmitTexture(
      TexFmt format, std::span<const uint8_t> buffer,
      glm::vec<2, size_t> dimensions) = 0;

  std::vector<uint8_t> GetSpriteSheetImage(SpriteSheet const& sheet) {
    std::vector<uint8_t> result(
        (size_t)(sheet.DesignWidth * sheet.DesignHeight * 4));
    GetSpriteSheetImage(sheet, result);
    return result;
  };

  virtual int GetSpriteSheetImage(SpriteSheet const& sheet,
                                  std::span<uint8_t> outBuffer) = 0;

  struct StateConfig {
    enum class BlendModeType : uint8_t {
      Normal,
      Additive,
      Premultiplied,
      Disabled
    };
    enum class StencilModeType : uint8_t { Off, Test, Write };

    BlendModeType BlendMode = BlendModeType::Normal;
    StencilModeType StencilMode = StencilModeType::Off;
    std::optional<RectF> ScissorRect = std::nullopt;
  };

  struct SpriteConfig {
    glm::vec3 ColorShift = glm::vec3(0.0f);
  };
  virtual void DrawSprite(
      const PositionedSprite& spriteInfo,
      const SpriteConfig& config = CreateDefault<SpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  struct InvertedSpriteConfig {};
  virtual void DrawInvertedSprite(
      const PositionedSprite& spriteInfo,
      const InvertedSpriteConfig& config =
          CreateDefault<InvertedSpriteConfig>(),
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  void DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                  glm::mat4 transformation, std::span<const glm::vec4, 4> tints,
                  glm::vec3 colorShift = glm::vec3(0.0f), bool inverted = false,
                  bool disableBlend = false, bool textureWrapRepeat = false);

  void DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                  glm::mat4 transformation = glm::mat4(1.0f),
                  glm::vec4 tint = glm::vec4(1.0f),
                  glm::vec3 colorShift = glm::vec3(0.0f), bool inverted = false,
                  bool disableBlend = false) {
    DrawSprite(sprite, dest, transformation,
               std::array<glm::vec4, 4>{tint, tint, tint, tint}, colorShift,
               inverted, disableBlend);
  }
  void DrawSprite(const Sprite& sprite, const CornersQuad& dest, glm::vec4 tint,
                  glm::vec3 colorShift = glm::vec3(0.0f), bool inverted = false,
                  bool disableBlend = false) {
    DrawSprite(sprite, dest, glm::mat4(1.0f), tint, colorShift, inverted,
               disableBlend);
  }
  void DrawSprite(const Sprite& sprite, glm::mat4 transformation,
                  glm::vec4 tint = glm::vec4(1.0f), bool inverted = false,
                  bool disableBlend = false) {
    DrawSprite(sprite, sprite.ScaledBounds(), transformation, tint,
               glm::vec3(0.0f), inverted, disableBlend);
  }
  void DrawSprite(const Sprite& sprite, glm::vec2 topLeft,
                  glm::vec4 tint = glm::vec4(1.0f), bool inverted = false,
                  bool disableBlend = false) {
    DrawSprite(sprite, sprite.ScaledBounds().Translate(topLeft),
               glm::mat4(1.0f), tint, glm::vec3(0.0f), inverted, disableBlend);
  }

  struct MaskedBinarySpriteConfig {
    bool IsInverted = false;
  };
  virtual void DrawMaskedBinarySprite(
      const PositionedMaskedSprite& spriteInfo,
      const MaskedBinarySpriteConfig& config,
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  struct MaskedSpriteConfig {
    float Alpha;
    float FadeRange;
    bool IsInverted = false;
  };
  virtual void DrawMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const MaskedSpriteConfig& config,
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  struct MaskedSpriteNoAlphaConfig {
    float Alpha;
    float FadeRange;
    bool IsInverted = false;
  };
  virtual void DrawMaskedSpriteNoAlpha(
      const PositionedMaskedSprite& spriteInfo,
      const MaskedSpriteNoAlphaConfig& config,
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  void DrawConvexShape(std::span<const glm::vec2> vertices,
                       glm::mat4 transformation, glm::vec4 color);
  void DrawConvexShape(std::span<const glm::vec2> vertices, glm::vec4 color) {
    DrawConvexShape(vertices, glm::mat4(1.0f), color);
  }

  void DrawQuad(const CornersQuad& dest, glm::vec4 color);

  struct CCMessageBoxConfig {
    int Alpha;
    int FadeRange;
    float EffectCt;
  };
  virtual void DrawCCMessageBox(
      const PositionedMaskedSprite& spriteInfo,
      const CCMessageBoxConfig& config,
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  struct EdgeDetectedSingleSheetFontConfig {
    float DifferenceFactor;
    float IntensityShift;
    float AlphaShift;
  };
  virtual void DrawEdgeDetectedSingleSheetFont(
      const PositionedSprite& spriteInfo,
      const EdgeDetectedSingleSheetFontConfig& config,
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;
  virtual void DrawEdgeDetectedSingleSheetFont(
      const PositionedMaskedSprite& spriteInfo,
      const EdgeDetectedSingleSheetFontConfig& config,
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  struct CHLCCMenuBackgroundConfig {};
  virtual void DrawCHLCCMenuBackground(
      const PositionedMaskedSprite& spriteInfo,
      const CHLCCMenuBackgroundConfig& =
          CreateDefault<CHLCCMenuBackgroundConfig>(),
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  struct BlurredSpriteConfig {
    RendererBlurDirection BlurDirection;
  };
  virtual void DrawBlurredSprite(
      const PositionedSprite& spriteInfo, const BlurredSpriteConfig& config,
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  struct MosaicConfig {
    float TileSize;
  };
  virtual void DrawMosaic(
      const PositionedSprite& spriteInfo, const MosaicConfig& config,
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  virtual void DrawVideoTexture(const YUVFrame& frame, const RectF& dest,
                                glm::vec4 tint, bool alphaVideo = false) = 0;
  virtual void DrawVideoTexture(const NV12Frame& frame, const RectF& dest,
                                glm::vec4 tint, bool alphaVideo = false) = 0;

  struct SilhouetteConfig {};
  virtual void DrawSilhouette(
      const PositionedSprite& spriteInfo,
      const SilhouetteConfig& config = CreateDefault<SilhouetteConfig>(),
      const StateConfig& stateconfig = CreateDefault<StateConfig>()) = 0;

  struct AdditiveMaskedSpriteConfig {};
  virtual void DrawAdditiveMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const AdditiveMaskedSpriteConfig& config =
          CreateDefault<AdditiveMaskedSpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  struct ColorBurnMaskedSpriteConfig {};
  virtual void DrawColorBurnMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const ColorBurnMaskedSpriteConfig& config =
          CreateDefault<ColorBurnMaskedSpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  struct ColorDodgeMaskedSpriteConfig {};
  virtual void DrawColorDodgeMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const ColorDodgeMaskedSpriteConfig& config =
          CreateDefault<ColorDodgeMaskedSpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  struct ColorMaskedSpriteConfig {};
  virtual void DrawColorMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const ColorMaskedSpriteConfig& config =
          CreateDefault<ColorMaskedSpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  struct HardLightMaskedSpriteConfig {};
  virtual void DrawHardLightMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const HardLightMaskedSpriteConfig& config =
          CreateDefault<HardLightMaskedSpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  struct LinearBurnMaskedSpriteConfig {};
  virtual void DrawLinearBurnMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const LinearBurnMaskedSpriteConfig& config =
          CreateDefault<LinearBurnMaskedSpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  struct OverlayMaskedSpriteConfig {};
  virtual void DrawOverlayMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const OverlayMaskedSpriteConfig& config =
          CreateDefault<OverlayMaskedSpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  struct ScreenMaskedSpriteConfig {};
  virtual void DrawScreenMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const ScreenMaskedSpriteConfig& config =
          CreateDefault<ScreenMaskedSpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  struct SoftLightMaskedSpriteConfig {};
  virtual void DrawSoftLightMaskedSprite(
      const PositionedMaskedSprite& spriteInfo,
      const SoftLightMaskedSpriteConfig& config =
          CreateDefault<SoftLightMaskedSpriteConfig>(),
      const StateConfig& stateConfig = CreateDefault<StateConfig>()) = 0;

  virtual void CaptureScreencap(Sprite& sprite) = 0;

  virtual void SetFrameBuffer(size_t buffer) = 0;
  virtual TextureRef GetFrameBufferTexture(size_t buffer) = 0;

  virtual void SetScissorRect(std::optional<RectF> rect) = 0;

  virtual void SetStencilMode(
      std::optional<StateConfig::StencilModeType> mode) = 0;
  virtual void ClearStencilBuffer() = 0;

  virtual void Clear(glm::vec4 color) = 0;

  IScene3D* Scene = 0;
  bool IsInit = false;

 protected:
  virtual void Flush() = 0;

  [[nodiscard]] virtual MutableTextureRef DeclareMutableTexture(
      TexFmt format, glm::vec<2, size_t> dimensions) = 0;

  static void QuadSetUV(CornersQuad spriteBounds, glm::vec2 designDimensions,
                        glm::vec2* uvs, size_t stride);

  static void QuadSetUVFlipped(CornersQuad spriteBounds,
                               glm::vec2 designDimensions, glm::vec2* uvs,
                               size_t stride) {
    QuadSetUV({spriteBounds.BottomLeft, spriteBounds.TopLeft,
               spriteBounds.BottomRight, spriteBounds.TopRight},
              designDimensions, uvs, stride);
  }

  void QuadSetPosition(CornersQuad destQuad, glm::vec2* positions, int stride);

  virtual glm::vec2 DesignToNDC(glm::vec2 designCoord) const {
    return designCoord;
  }

  Sprite RectSprite;

  friend class YUVFrame;
  friend class NV12Frame;
};

inline void InsertQuad(std::span<VertexBufferSprites, 4> vertices,
                       CornersQuad position, CornersQuad uv, glm::vec4 tint,
                       CornersQuad maskUV = RectF()) {
  vertices[0] = {
      .Position = position.BottomLeft,
      .UV = uv.BottomLeft,
      .Tint = tint,
      .MaskUV = maskUV.BottomLeft,
  };
  vertices[1] = {
      .Position = position.TopLeft,
      .UV = uv.TopLeft,
      .Tint = tint,
      .MaskUV = maskUV.TopLeft,
  };
  vertices[2] = {
      .Position = position.TopRight,
      .UV = uv.TopRight,
      .Tint = tint,
      .MaskUV = maskUV.TopRight,
  };
  vertices[3] = {
      .Position = position.BottomRight,
      .UV = uv.BottomRight,
      .Tint = tint,
      .MaskUV = maskUV.BottomRight,
  };
}

inline std::unique_ptr<BaseRenderer> Renderer;
inline std::unique_ptr<BaseWindow> Window;

void CreateRenderer();

}  // namespace Impacto
