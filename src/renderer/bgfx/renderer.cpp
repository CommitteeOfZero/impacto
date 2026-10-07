#include "renderer.h"

#include "../../log.h"
#include "../../userconfig.h"

#include <bgfx/shader_content.h>

#include <bgfx/platform.h>
#include <glm/gtc/type_ptr.hpp>

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui_impl_bgfx.h>
#endif

#ifdef __SWITCH__
extern "C" {
#include <switch/display/native_window.h>
}
#endif
namespace Impacto::Bgfx {

constexpr bgfx::ViewId RENDER_VIEW = 0;   // Uses render dimensions
constexpr bgfx::ViewId DISPLAY_VIEW = 1;  // Uses viewport dimensions
constexpr bgfx::ViewId IMGUI_VIEW = 255;

Renderer::Renderer() {
  bgfx::Init initStruct{};

  initStruct.type = []() -> bgfx::RendererType::Enum {
    switch (UserConfig::AdvancedSettings.ActiveRenderer) {
#ifdef IMPACTO_RENDERER_OPENGL
      case RendererType::OpenGL:
        return bgfx::RendererType::OpenGL;
#endif
#ifdef IMPACTO_RENDERER_OPENGLES
      case RendererType::OpenGLES:
        return bgfx::RendererType::OpenGLES;
#endif
#ifdef IMPACTO_RENDERER_VULKAN
      case RendererType::Vulkan:
        return bgfx::RendererType::Vulkan;
#endif
#ifdef IMPACTO_RENDERER_DIRECT3D11
      case RendererType::Direct3D11:
        return bgfx::RendererType::Direct3D11;
#endif
#ifdef IMPACTO_RENDERER_DIRECT3D12
      case RendererType::Direct3D12:
        return bgfx::RendererType::Direct3D12;
#endif
#ifdef IMPACTO_RENDERER_METAL
      case RendererType::Metal:
        return bgfx::RendererType::Metal;
#endif

      default:
        assert(false && "Unsupported bgfx render type");
        return bgfx::RendererType::Count;  // Have bgfx choose
    }
  }();
  ImpLog(LogLevel::Info, LogChannel::Render,
         "Initializing BGFX with {:s} backend",
         magic_enum::enum_name(initStruct.type));

  initStruct.resolution.width = UserConfig::CommonSettings.WindowWidth;
  initStruct.resolution.height = UserConfig::CommonSettings.WindowHeight;
  initStruct.resolution.reset = BGFX_RESET_VSYNC;
#if defined(SDL_PLATFORM_WIN32)
  initStruct.platformData.nwh =
      SDL_GetPointerProperty(SDL_GetWindowProperties(Window->SDLWindow),
                             SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
#elif defined(SDL_PLATFORM_LINUX)
  if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
    initStruct.platformData.ndt =
        SDL_GetPointerProperty(SDL_GetWindowProperties(Window->SDLWindow),
                               SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
    initStruct.platformData.nwh = std::bit_cast<void*>(
        SDL_GetNumberProperty(SDL_GetWindowProperties(Window->SDLWindow),
                              SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
  } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
    initStruct.platformData.ndt = SDL_GetPointerProperty(
        SDL_GetWindowProperties(Window->SDLWindow),
        SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
    initStruct.platformData.nwh = SDL_GetPointerProperty(
        SDL_GetWindowProperties(Window->SDLWindow),
        SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
  } else {
    Panic(LogChannel::Render, "Unsupported video driver \"{:s}\"",
          SDL_GetCurrentVideoDriver());
  }
#elif defined(SDL_PLATFORM_ANDROID)
  initStruct.platformData.nwh =
      SDL_GetPointerProperty(SDL_GetWindowProperties(Window->SDLWindow),
                             SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);
#elif defined(SDL_PLATFORM_MACOS)
  initStruct.platformData.nwh =
      SDL_GetPointerProperty(SDL_GetWindowProperties(Window->SDLWindow),
                             SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
#elif defined(SDL_PLATFORM_SWITCH)
  initStruct.platformData.nwh = nwindowGetDefault();
#else
  static_assert(false && "We have not implemented BGFX for this platform");
#endif

#if IMPACTO_GL_DEBUG
  initStruct.debug = true;
#else
  initStruct.debug = false;
#endif

  if (!bgfx::init(initStruct)) {
    Panic(LogChannel::Render, "Failed to initialize BGFX");
  }

  constexpr static glm::mat4 identityMatrix(1.0f);
  bgfx::setViewTransform(DISPLAY_VIEW, glm::value_ptr(identityMatrix),
                         glm::value_ptr(identityMatrix));

  constexpr uint32_t black = 0x000000ff;
  bgfx::setViewClear(RENDER_VIEW, BGFX_CLEAR_COLOR | BGFX_CLEAR_STENCIL, black);
  bgfx::setViewClear(DISPLAY_VIEW, BGFX_CLEAR_COLOR, black);

  IndexBuffer = bgfx::createDynamicIndexBuffer(static_cast<uint32_t>(0),
                                               BGFX_BUFFER_ALLOW_RESIZE);
  if (!bgfx::isValid(IndexBuffer)) {
    Panic(LogChannel::Render, "Failed to create index buffer");
  }

  VertexBufferSpritesLayout.begin()
      .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float, true)
      .add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float)
      .end();

  VertexBuffer = bgfx::createDynamicVertexBuffer(static_cast<uint32_t>(0),
                                                 VertexBufferSpritesLayout,
                                                 BGFX_BUFFER_ALLOW_RESIZE);
  if (!bgfx::isValid(VertexBuffer)) {
    Panic(LogChannel::Render, "Failed to create vertex buffer");
  }

  const auto flush = [this]() { Flush(); };
  CCMessageBoxSpriteShader.emplace(vs_maskedsprite_shader,
                                   fs_ccmessageboxsprite_shader, flush);
  CHLCCMenuBackgroundShader.emplace(vs_maskedsprite_shader,
                                    fs_chlccmenubackground_shader, flush);
  EdgeDetectedSingleSheetFontShader.emplace(
      vs_maskedsprite_shader, fs_edgedetectedsinglesheetfont_shader, flush);
  GaussianBlurShader.emplace(vs_sprite_shader, fs_gaussianblur_shader, flush);
  MaskedSpriteShader.emplace(vs_maskedsprite_shader, fs_maskedsprite_shader,
                             flush);
  MaskedSpriteBinaryShader.emplace(vs_maskedsprite_shader,
                                   fs_maskedspritebinary_shader, flush);
  MaskedSpriteNoAlphaShader.emplace(vs_maskedsprite_shader,
                                    fs_maskedspritenoalpha_shader, flush);
  MosaicShader.emplace(vs_sprite_shader, fs_mosaic_shader, flush);
  NV12FrameShader.emplace(vs_sprite_shader, fs_nv12frame_shader, flush);
  SilhouetteShader.emplace(vs_sprite_shader, fs_silhouette_shader, flush);
  SpriteShader.emplace(vs_sprite_shader, fs_sprite_shader, flush);
  YUVFrameShader.emplace(vs_sprite_shader, fs_yuvframe_shader, flush);

  RectSprite = Sprite(SpriteSheet(1.0f, 1.0f), 0.0f, 0.0f, 1.0f, 1.0f);

  ImGui_Implbgfx_Init(IMGUI_VIEW);
  switch (UserConfig::AdvancedSettings.ActiveRenderer) {
#ifdef IMPACTO_RENDERER_OPENGL
    case RendererType::OpenGL:
      ImGui_ImplSDL3_InitForOpenGL(Window->SDLWindow, nullptr);
      break;
#endif
#ifdef IMPACTO_RENDERER_OPENGLES
    case RendererType::OpenGLES:
      ImGui_ImplSDL3_InitForOpenGL(Window->SDLWindow, nullptr);
      break;
#endif
#ifdef IMPACTO_RENDERER_VULKAN
    case RendererType::Vulkan:
      ImGui_ImplSDL3_InitForVulkan(Window->SDLWindow);
      break;
#endif
#ifdef IMPACTO_RENDERER_DIRECT3D11
    case RendererType::Direct3D11:
      ImGui_ImplSDL3_InitForD3D(Window->SDLWindow);
      break;
#endif
#ifdef IMPACTO_RENDERER_DIRECT3D12
    case RendererType::Direct3D12:
      ImGui_ImplSDL3_InitForD3D(Window->SDLWindow);
      break;
#endif
#ifdef IMPACTO_RENDERER_METAL
    case RendererType::Metal:
      ImGui_ImplSDL3_InitForMetal(Window->SDLWindow);
      break;
#endif

    default:
      assert(false);
      break;
  }
}

void Renderer::Init() {
  [[maybe_unused]] const RendererType type = GetType();

  UpdateResolution();
  DrawFrameBuffer = FrameBuffer(static_cast<uint16_t>(Resolution.x),
                                static_cast<uint16_t>(Resolution.y));

  const glm::mat4 projectionMatrix =
      glm::ortho(0.0f, Profile::Game::DesignWidth, Profile::Game::DesignHeight,
                 0.0f, -Profile::Game::DesignWidth, Profile::Game::DesignWidth);
  constexpr static glm::mat4 identityMatrix(1.0f);
  bgfx::setViewTransform(RENDER_VIEW, glm::value_ptr(identityMatrix),
                         glm::value_ptr(projectionMatrix));

  constexpr static std::array<uint16_t, 6> backBufferIndices = {
      0, 1, 3, 1, 2, 3,
  };
  BackBufferIndexBuffer = bgfx::createIndexBuffer(
      bgfx::makeRef(backBufferIndices.data(), sizeof(backBufferIndices)));

  const bool shouldFlip = false
#ifdef IMPACTO_RENDERER_OPENGL
                          || type == RendererType::OpenGL
#endif
#ifdef IMPACTO_RENDERER_OPENGLES
                          || type == RendererType::OpenGLES
#endif
      ;
  constexpr static std::array<VertexBufferSprites, 4> backBufferVertices = {
      VertexBufferSprites{.Position = {-1.0f, +1.0f}, .UV = {0.0f, 0.0f}},
      VertexBufferSprites{.Position = {-1.0f, -1.0f}, .UV = {0.0f, 1.0f}},
      VertexBufferSprites{.Position = {+1.0f, -1.0f}, .UV = {1.0f, 1.0f}},
      VertexBufferSprites{.Position = {+1.0f, +1.0f}, .UV = {1.0f, 0.0f}},
  };
  constexpr static std::array<VertexBufferSprites, 4>
      flippedBackBufferVertices = {
          VertexBufferSprites{.Position = {-1.0f, -1.0f}, .UV = {0.0f, 0.0f}},
          VertexBufferSprites{.Position = {-1.0f, +1.0f}, .UV = {0.0f, 1.0f}},
          VertexBufferSprites{.Position = {+1.0f, +1.0f}, .UV = {1.0f, 1.0f}},
          VertexBufferSprites{.Position = {+1.0f, -1.0f}, .UV = {1.0f, 0.0f}},
  };
  const std::span<const VertexBufferSprites, 4> correctBackBufferVertices =
      shouldFlip ? flippedBackBufferVertices : backBufferVertices;
  BackBufferVertexBuffer = bgfx::createVertexBuffer(
      bgfx::makeRef(
          correctBackBufferVertices.data(),
          static_cast<uint32_t>(correctBackBufferVertices.size_bytes())),
      VertexBufferSpritesLayout);

  RectSprite.Sheet.Texture =
      SubmitTexture(TexFmt::TexFmt_RGBA,
                    std::array<uint8_t, 4>{0xFF, 0xFF, 0xFF, 0xFF}, {1, 1});
}

void Renderer::UpdateResolution() {
  const auto& gameConfig = UserConfig::ActiveGameSettings();

  if (gameConfig.ResolutionWidth.has_value() !=
      gameConfig.ResolutionHeight.has_value()) {
    ImpLog(LogLevel::Warning, LogChannel::Render,
           "Only one of Resolution Height or Resolution Width is configured, "
           "defaulting to native game resolution.");
  }

  // Queue up new resolution for seamless switching between the end of the
  // current frame and the start of the next
  Resolution =
      gameConfig.ResolutionWidth.has_value() &&
              gameConfig.ResolutionHeight.has_value()
          ? glm::ivec2{*gameConfig.ResolutionWidth,
                       *gameConfig.ResolutionHeight}
          : glm::ivec2{Profile::Game::DesignWidth, Profile::Game::DesignHeight};
}

RendererType Renderer::GetType() const {
  switch (bgfx::getRendererType()) {
    using enum bgfx::RendererType::Enum;
#ifdef IMPACTO_RENDERER_DIRECT3D11
    case Direct3D11:
      return RendererType::Direct3D11;
      break;
#endif
#ifdef IMPACTO_RENDERER_DIRECT3D12
    case Direct3D12:
      return RendererType::Direct3D12;
#endif
#ifdef IMPACTO_RENDERER_METAL
    case Metal:
      return RendererType::Metal;
#endif
#ifdef IMPACTO_RENDERER_OPENGL
    case OpenGL:
      return RendererType::OpenGL;
#endif
#ifdef IMPACTO_RENDERER_OPENGLES
    case OpenGLES:
      return RendererType::OpenGLES;
#endif
#ifdef IMPACTO_RENDERER_VULKAN
    case Vulkan:
      return RendererType::Vulkan;
#endif

    default:
      break;
  }

  Panic(LogChannel::Render, "Unexpected bgfx renderer \"{:s}\"\n",
        bgfx::getRendererName(bgfx::getRendererType()));
}

void Renderer::BeginFrame() {
  OrphanedTextures.clear();

  bgfx::reset(static_cast<uint32_t>(Window->WindowWidth),
              static_cast<uint32_t>(Window->WindowHeight), BGFX_RESET_VSYNC);

  bgfx::setViewRect(DISPLAY_VIEW, 0, 0,
                    static_cast<uint16_t>(Window->WindowWidth),
                    static_cast<uint16_t>(Window->WindowHeight));

  bgfx::touch(DISPLAY_VIEW);
}

void Renderer::BeginFrame2D() {
  if (DrawFrameBuffer.GetSize() != Resolution) {
    DrawFrameBuffer = FrameBuffer(static_cast<uint16_t>(Resolution.x),
                                  static_cast<uint16_t>(Resolution.y));
  }

  bgfx::setViewFrameBuffer(RENDER_VIEW, DrawFrameBuffer);
  bgfx::setViewRect(RENDER_VIEW, 0, 0, static_cast<uint16_t>(Resolution.x),
                    static_cast<uint16_t>(Resolution.y));

  bgfx::touch(RENDER_VIEW);
}

void Renderer::EndFrame() {
  Flush();

  assert(Indices.empty() == Vertices.empty());
  if (!Indices.empty()) {
    bgfx::update(IndexBuffer, 0,
                 bgfx::copy(Indices.data(),
                            static_cast<uint32_t>(Indices.size() *
                                                  sizeof(Indices.front()))));
    bgfx::update(VertexBuffer, 0,
                 bgfx::copy(Vertices.data(),
                            static_cast<uint32_t>(Vertices.size() *
                                                  sizeof(Vertices.front()))));

    Indices.clear();
    Vertices.clear();

    CurFrameIndexBufferOffset = 0;
    CurFrameVertexBufferOffset = 0;
  }

  SetState({
      .GenericState = StateConfig{},
      .ShaderProgram = *SpriteShader,
  });

  bgfx::setIndexBuffer(BackBufferIndexBuffer);
  bgfx::setVertexBuffer(0, BackBufferVertexBuffer);

  SpriteShader->SubmitUniforms({}, {.s_texture = DrawFrameBuffer.GetTexture()});
  bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_DEPTH_TEST_ALWAYS);

  bgfx::submit(DISPLAY_VIEW, *SpriteShader);
}

#ifndef IMPACTO_DISABLE_IMGUI
void Renderer::ImGuiBeginFrame() {
  ImGui_Implbgfx_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  bgfx::touch(IMGUI_VIEW);
}
#endif

decltype(Renderer::Textures)::iterator Renderer::DeclareTexture(
    std::unique_ptr<Texture>&& texture) {
  const uint64_t textureId = texture->GetTextureHandle().idx;

  assert(!Textures.contains(textureId));
  return Textures.emplace(textureId, std::pair{std::move(texture), 0}).first;
}

TextureRef Renderer::SubmitTexture(const TexFmt format,
                                   const std::span<const uint8_t> buffer,
                                   const glm::vec<2, size_t> dimensions) {
  std::unique_ptr<Texture> texture =
      std::make_unique<Texture>(TexFmtConversion[format], buffer, dimensions);
  Texture* const texturePtr = texture.get();

  DeclareTexture(std::move(texture));
  return TextureRef(texturePtr);
}

MutableTextureRef Renderer::DeclareMutableTexture(
    const TexFmt format, const glm::vec<2, size_t> dimensions) {
  std::unique_ptr<MutableTexture> texture =
      std::make_unique<MutableTexture>(TexFmtConversion[format], dimensions);
  MutableTexture* const texturePtr = texture.get();

  DeclareTexture(std::move(texture));
  return MutableTextureRef(texturePtr);
}

void Renderer::Shutdown() {
  if (bgfx::isValid(IndexBuffer)) bgfx::destroy(IndexBuffer);
  IndexBuffer.idx = bgfx::kInvalidHandle;

  if (bgfx::isValid(VertexBuffer)) bgfx::destroy(VertexBuffer);
  VertexBuffer.idx = bgfx::kInvalidHandle;

  if (bgfx::isValid(BackBufferIndexBuffer)) {
    bgfx::destroy(BackBufferIndexBuffer);
  }
  BackBufferIndexBuffer.idx = bgfx::kInvalidHandle;

  if (bgfx::isValid(BackBufferVertexBuffer)) {
    bgfx::destroy(BackBufferVertexBuffer);
  }
  BackBufferVertexBuffer.idx = bgfx::kInvalidHandle;

#ifndef IMPACTO_DISABLE_IMGUI
  ImGui_ImplSDL3_Shutdown();
  ImGui_Implbgfx_Shutdown();
  ImGui::DestroyContext();
#endif
}

void Renderer::Flush() {
  const bool empty = Indices.size() == CurFrameIndexBufferOffset;
  assert(empty == (Vertices.size() == CurFrameVertexBufferOffset));
  if (empty || !CurrentState.has_value()) return;

  bgfx::setIndexBuffer(
      IndexBuffer, static_cast<uint32_t>(CurFrameIndexBufferOffset),
      static_cast<uint32_t>(Indices.size() - CurFrameIndexBufferOffset));
  bgfx::setVertexBuffer(
      0, VertexBuffer, static_cast<uint32_t>(CurFrameVertexBufferOffset),
      static_cast<uint32_t>(Vertices.size() - CurFrameVertexBufferOffset));

  CurFrameIndexBufferOffset = Indices.size();
  CurFrameVertexBufferOffset = Vertices.size();

  {
    uint64_t stateFlags = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;

    switch (CurrentState->GenericState.BlendMode) {
      using enum StateConfig::BlendModeType;
      case Normal:
        stateFlags |= BGFX_STATE_BLEND_FUNC_SEPARATE(
            BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA,
            BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
        break;
      case Additive:
        stateFlags |= BGFX_STATE_BLEND_FUNC_SEPARATE(
            BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_ONE,
            BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
        break;
      case Premultiplied:
        stateFlags |= BGFX_STATE_BLEND_NORMAL;
        break;
    }

    switch (CurrentState->GenericState.StencilMode) {
      using enum StateConfig::StencilModeType;
      case Off:
        bgfx::setStencil(BGFX_STENCIL_NONE);
        break;
      case Test:
        bgfx::setStencil(
            BGFX_STENCIL_TEST_NOTEQUAL | BGFX_STENCIL_OP_FAIL_S_KEEP |
            BGFX_STENCIL_OP_FAIL_Z_KEEP | BGFX_STENCIL_OP_PASS_Z_KEEP |
            BGFX_STENCIL_FUNC_REF(0) | BGFX_STENCIL_FUNC_RMASK(0xFF));
        break;
      case Write:
        bgfx::setStencil(
            BGFX_STENCIL_TEST_NEVER | BGFX_STENCIL_OP_FAIL_S_REPLACE |
            BGFX_STENCIL_OP_FAIL_Z_REPLACE | BGFX_STENCIL_OP_PASS_Z_REPLACE |
            BGFX_STENCIL_FUNC_REF(1) | BGFX_STENCIL_FUNC_RMASK(0xFF));
        break;
    }

    bgfx::setState(stateFlags);
  }

  bgfx::submit(RENDER_VIEW, CurrentState->ShaderProgram.get());
}

bool Renderer::ShouldFlip(const Texture& texture) const {
  [[maybe_unused]] const RendererType rendererType =
      Impacto::Renderer->GetType();
  return texture.IsScreenCap() && (false
#ifdef IMPACTO_RENDERER_OPENGL
                                   || rendererType == RendererType::OpenGL
#endif
#ifdef IMPACTO_RENDERER_OPENGLES
                                   || rendererType == RendererType::OpenGLES
#endif
                                  );
}

void Renderer::InsertVertices(
    const std::span<const uint16_t> indices,
    const std::span<const VertexBufferSprites> vertices, bool flipVertically) {
  assert(indices.empty() == vertices.empty());
  if (indices.empty()) return;

  static uint16_t curIndex = 0;
  if (Indices.size() == CurFrameIndexBufferOffset) curIndex = 0;

  Indices.resize(Indices.size() + indices.size());
  std::ranges::transform(
      indices, Indices.begin() + (Indices.size() - indices.size()),
      [&](const uint16_t index) { return index + curIndex; });
  curIndex += std::ranges::max(indices) + 1;

  if (!flipVertically) {
    Vertices.insert(Vertices.end(), vertices.begin(), vertices.end());
  } else {
    Vertices.resize(Vertices.size() + vertices.size());
    std::ranges::transform(
        vertices, Vertices.begin() + (Vertices.size() - vertices.size()),
        [](VertexBufferSprites vertex) {
          vertex.Position.y = Profile::Game::DesignHeight - vertex.Position.y;
          return vertex;
        });
  }
}

void Renderer::SetState(const RendererState& newState) {
  if (!CurrentState.has_value() ||
      CurrentState->Transformation != newState.Transformation) {
    Flush();
    bgfx::setTransform(glm::value_ptr(newState.Transformation));
  }

  if (CurrentState.has_value() &&
      &CurrentState->ShaderProgram.get() != &newState.ShaderProgram.get()) {
    Flush();
  }

  if (!CurrentState.has_value() ||
      CurrentState->GenericState.BlendMode != newState.GenericState.BlendMode) {
    Flush();
  }

  if (!CurrentState.has_value() || CurrentState->GenericState.StencilMode !=
                                       newState.GenericState.StencilMode) {
    Flush();
  }

  if (!CurrentState.has_value() || CurrentState->GenericState.ScissorRect !=
                                       newState.GenericState.ScissorRect) {
    Flush();

    if (newState.GenericState.ScissorRect.has_value()) {
      const RectF& scissorRect = *newState.GenericState.ScissorRect;
      bgfx::setScissor(static_cast<uint16_t>(scissorRect.X),
                       static_cast<uint16_t>(scissorRect.Y),
                       static_cast<uint16_t>(scissorRect.Width),
                       static_cast<uint16_t>(scissorRect.Height));
    } else {
      // All zeroes is defined as disable scissor
      bgfx::setScissor(0, 0, 0, 0);
    }
  }

  CurrentState = newState;
}

void Renderer::DrawCCMessageBox(const PositionedMaskedSprite& spriteInfo,
                                const CCMessageBoxConfig& config,
                                const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *CCMessageBoxSpriteShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);
  const Texture& maskTexture =
      dynamic_cast<const Texture&>(spriteInfo.MaskTexture);

  CCMessageBoxSpriteShader->SubmitUniforms(
      {
          .u_maskTransformation = spriteInfo.MaskTransformation,
          .u_fullscreenMask = spriteInfo.FullscreenMask,
      },
      {
          .s_texture = texture,
          .s_mask = maskTexture,
          .u_alpha = glm::vec3(config.FadeRange, config.Alpha, config.EffectCt),
      });

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawCHLCCMenuBackground(const PositionedMaskedSprite& spriteInfo,
                                       const CHLCCMenuBackgroundConfig& config,
                                       const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *CHLCCMenuBackgroundShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);
  const Texture& maskTexture =
      dynamic_cast<const Texture&>(spriteInfo.MaskTexture);

  CHLCCMenuBackgroundShader->SubmitUniforms(
      {
          .u_maskTransformation = spriteInfo.MaskTransformation,
          .u_fullscreenMask = spriteInfo.FullscreenMask,
      },
      {
          .s_texture = texture,
          .s_mask = maskTexture,
      });

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawEdgeDetectedSingleSheetFont(
    const PositionedSprite& spriteInfo,
    const EdgeDetectedSingleSheetFontConfig& config,
    const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *EdgeDetectedSingleSheetFontShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);

  EdgeDetectedSingleSheetFontShader->SubmitUniforms(
      {
          .u_maskTransformation = glm::mat4(1.0f),
          .u_fullscreenMask = false,
      },
      {
          .s_font = texture,
          .s_mask = bgfx::TextureHandle{bgfx::kInvalidHandle},
          .u_hasMask = false,
          .u_strength = glm::vec3(config.DifferenceFactor,
                                  config.IntensityShift, config.AlphaShift),
      });

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawEdgeDetectedSingleSheetFont(
    const PositionedMaskedSprite& spriteInfo,
    const EdgeDetectedSingleSheetFontConfig& config,
    const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *EdgeDetectedSingleSheetFontShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);
  const Texture& maskTexture =
      dynamic_cast<const Texture&>(spriteInfo.MaskTexture);

  EdgeDetectedSingleSheetFontShader->SubmitUniforms(
      {
          .u_maskTransformation = spriteInfo.MaskTransformation,
          .u_fullscreenMask = spriteInfo.FullscreenMask,
      },
      {
          .s_font = texture,
          .s_mask = maskTexture,
          .u_hasMask = true,
          .u_strength = glm::vec3(config.DifferenceFactor,
                                  config.IntensityShift, config.AlphaShift),
      });

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawBlurredSprite(const PositionedSprite& spriteInfo,
                                 const BlurredSpriteConfig& config,
                                 const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *GaussianBlurShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);

  GaussianBlurShader->SubmitUniforms(
      {}, {
              .s_texture = texture,
              .u_isHorizontal =
                  config.BlurDirection == RendererBlurDirection::Horizontal,
          });

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawSprite(const Sprite& sprite, const CornersQuad& dest,
                          const glm::mat4 transformation,
                          const std::span<const glm::vec4, 4> tints,
                          const glm::vec3 colorShift, const bool inverted,
                          const bool disableBlend,
                          const bool textureWrapRepeat) {
  if (std::ranges::all_of(
          tints, [](float alpha) { return alpha <= 0.0f; }, &glm::vec4::a)) {
    return;
  }

  SetState({
      .GenericState = StateConfig{},
      .ShaderProgram = *SpriteShader,
      .Transformation = transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(*sprite.Sheet.Texture);

  SpriteShader->SubmitUniforms({}, {
                                       .s_texture = texture,
                                       .u_colorShift = colorShift,
                                   });

  InsertQuad(dest, sprite.NormalizedBounds(), tints, ShouldFlip(texture));
}

void Renderer::DrawMaskedSprite(const PositionedMaskedSprite& spriteInfo,
                                const MaskedSpriteConfig& config,
                                const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *MaskedSpriteShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);
  const Texture& maskTexture =
      dynamic_cast<const Texture&>(spriteInfo.MaskTexture);

  MaskedSpriteShader->SubmitUniforms(
      {
          .u_maskTransformation = spriteInfo.MaskTransformation,
          .u_fullscreenMask = spriteInfo.FullscreenMask,
      },
      {
          .s_texture = texture,
          .s_mask = maskTexture,
          .u_alpha = glm::vec2(config.Alpha, config.FadeRange),
          .u_isInverted = config.IsInverted,
          .u_isSameTexture = texture == maskTexture,
      });

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawMaskedBinarySprite(const PositionedMaskedSprite& spriteInfo,
                                      const MaskedBinarySpriteConfig& config,
                                      const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *MaskedSpriteBinaryShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);

  MaskedSpriteBinaryShader->SubmitUniforms(
      {
          .u_maskTransformation = spriteInfo.MaskTransformation,
          .u_fullscreenMask = spriteInfo.FullscreenMask,
      },
      {
          .s_texture = texture,
          .s_mask = dynamic_cast<const Texture&>(spriteInfo.MaskTexture),
          .u_isInverted = config.IsInverted,
      });

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawMaskedSpriteNoAlpha(const PositionedMaskedSprite& spriteInfo,
                                       const MaskedSpriteNoAlphaConfig& config,
                                       const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *MaskedSpriteNoAlphaShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);

  MaskedSpriteNoAlphaShader->SubmitUniforms(
      {
          .u_maskTransformation = spriteInfo.MaskTransformation,
          .u_fullscreenMask = spriteInfo.FullscreenMask,
      },
      {
          .s_texture = texture,
          .s_mask = dynamic_cast<const Texture&>(spriteInfo.MaskTexture),
          .u_alpha = glm::vec2(config.Alpha, config.FadeRange),
          .u_isInverted = config.IsInverted,
      });

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawMosaic(const PositionedSprite& spriteInfo,
                          const MosaicConfig& config,
                          const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *MosaicShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);

  MosaicShader->SubmitUniforms({}, {
                                       .s_texture = texture,
                                       .u_tileSize = config.TileSize,
                                   });

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawSilhouette(const PositionedSprite& spriteInfo,
                              const SilhouetteConfig& config,
                              const StateConfig& stateConfig) {
  SetState({
      .GenericState = stateConfig,
      .ShaderProgram = *SilhouetteShader,
      .Transformation = spriteInfo.Transformation,
  });

  const Texture& texture = dynamic_cast<const Texture&>(spriteInfo.Texture);

  SilhouetteShader->SubmitUniforms({}, {.s_coverageMap = texture});

  InsertVertices(spriteInfo.GetIndices(), spriteInfo.GetVertices(),
                 ShouldFlip(texture));
}

void Renderer::DrawPrimitives(
    const SpriteSheet& sheet, const SpriteSheet* const mask,
    const ShaderProgramType shaderType,
    const std::span<const VertexBufferSprites> vertices,
    const std::span<const uint16_t> indices,
    const glm::mat4 spriteTransformation, const glm::mat4 maskTransformation,
    const bool inverted, const TopologyMode topology,
    const bool textureWrapRepeat) {
  const Texture& texture = dynamic_cast<const Texture&>(*sheet.Texture);

  ShaderProgramInterface* const shader = [&]() -> ShaderProgramInterface* {
    switch (shaderType) {
      case ShaderProgramType::Sprite:
        SpriteShader->SubmitUniforms({}, {.s_texture = texture});
        return &*SpriteShader;
      default:
        break;
    }
    return nullptr;
  }();

  if (shader == nullptr) return;

  SetState({
      .GenericState = StateConfig{},
      .ShaderProgram = *shader,
      .Transformation = spriteTransformation,
  });

  InsertVertices(indices, vertices, ShouldFlip(texture));
}

void Renderer::DrawVideoTexture(const YUVFrame& frame, const RectF& dest,
                                const glm::vec4 tint, const bool alphaVideo) {
  SetState({
      .GenericState = StateConfig{},
      .ShaderProgram = *YUVFrameShader,
  });

  YUVFrameShader->SubmitUniforms(
      {}, {
              .s_luma = static_cast<MutableTexture&>(frame.GetLuma()),
              .s_cb = static_cast<MutableTexture&>(frame.GetCb()),
              .s_cr = static_cast<MutableTexture&>(frame.GetCr()),
              .u_isAlpha = alphaVideo,
          });

  InsertQuad(dest, RectF(0.0f, 0.0f, 1.0f, 1.0f), tint, false);
}

void Renderer::DrawVideoTexture(const NV12Frame& frame, const RectF& dest,
                                const glm::vec4 tint, const bool alphaVideo) {
  SetState({
      .GenericState = StateConfig{},
      .ShaderProgram = *NV12FrameShader,
  });

  NV12FrameShader->SubmitUniforms(
      {}, {
              .s_luma = static_cast<MutableTexture&>(frame.GetLuma()),
              .s_cbCr = static_cast<MutableTexture&>(frame.GetCbCr()),
              .u_isAlpha = alphaVideo,
          });

  InsertQuad(dest, RectF(0.0f, 0.0f, 1.0f, 1.0f), tint, false);
}

void Renderer::InsertQuad(const CornersQuad dest, const CornersQuad uvs,
                          const std::span<const glm::vec4, 4> tints,
                          const bool flipVertically,
                          const CornersQuad maskUvs) {
  constexpr static std::array<uint16_t, 6> indices = {0, 1, 3, 1, 2, 3};
  const std::array<VertexBufferSprites, 4> vertices = {
      VertexBufferSprites{
          .Position = dest.TopLeft,
          .UV = uvs.TopLeft,
          .Tint = tints[0],
          .MaskUV = maskUvs.TopLeft,
      },
      VertexBufferSprites{
          .Position = dest.BottomLeft,
          .UV = uvs.BottomLeft,
          .Tint = tints[1],
          .MaskUV = maskUvs.BottomLeft,
      },
      VertexBufferSprites{
          .Position = dest.BottomRight,
          .UV = uvs.BottomRight,
          .Tint = tints[2],
          .MaskUV = maskUvs.BottomRight,
      },
      VertexBufferSprites{
          .Position = dest.TopRight,
          .UV = uvs.TopRight,
          .Tint = tints[3],
          .MaskUV = maskUvs.TopRight,
      },
  };

  InsertVertices(indices, vertices, flipVertically);
}

void Renderer::AlterRefCount(TextureInterface* const texture,
                             const int difference) {
  assert(texture != nullptr);
  if (difference == 0) return;

  const auto textureIt = Textures.find(texture->GetTextureId());
  assert(textureIt != Textures.end() &&
         "Tried to alter the refcount of an already deleted texture.");
  size_t& refCount = textureIt->second.second;

  assert(difference >= 0 || static_cast<size_t>(-difference) <= refCount);
  refCount += difference;

  if (refCount == 0) {
    std::unique_ptr<Texture>& textureObject = textureIt->second.first;
    OrphanedTextures.emplace_back(std::move(textureObject));

    Textures.erase(textureIt);
  }
}

}  // namespace Impacto::Bgfx
