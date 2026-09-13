#include "renderer.h"

#include "../../log.h"
#include "../../userconfig.h"

#include <bgfx/shader_content.h>

#include <bgfx/platform.h>
#include <glm/gtc/type_ptr.hpp>

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui_impl_bgfx.h>
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

  initStruct.resolution.width = UserConfig::CommonSettings.ResolutionWidth;
  initStruct.resolution.height = UserConfig::CommonSettings.ResolutionHeight;
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
  bgfx::setViewClear(DISPLAY_VIEW, BGFX_CLEAR_COLOR, black);

  constexpr uint32_t transparentWhite = 0xffffff00;
  bgfx::setViewClear(RENDER_VIEW, BGFX_CLEAR_COLOR | BGFX_CLEAR_STENCIL,
                     transparentWhite);

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
  SpriteShader.emplace(vs_sprite_shader, fs_sprite_shader, flush);

  RectSprite = Sprite(SpriteSheet(1.0f, 1.0f), 0.0f, 0.0f, 1.0f, 1.0f);
  RectSprite.Sheet.Texture =
      SubmitTexture(TexFmt::TexFmt_RGBA,
                    std::array<uint8_t, 4>{0xFF, 0xFF, 0xFF, 0xFF}, 1, 1);

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

  DrawFrameBuffer =
      FrameBuffer(static_cast<uint16_t>(Profile::Game::DesignWidth),
                  static_cast<uint16_t>(Profile::Game::DesignHeight));

  const glm::mat4 projectionMatrix =
      glm::ortho(0.0f, Profile::Game::DesignWidth, Profile::Game::DesignHeight,
                 0.0f, -Profile::Game::DesignWidth, Profile::Game::DesignWidth);
  constexpr static glm::mat4 identityMatrix(1.0f);
  bgfx::setViewTransform(RENDER_VIEW, glm::value_ptr(identityMatrix),
                         glm::value_ptr(projectionMatrix));

  bgfx::setViewRect(RENDER_VIEW, 0, 0,
                    static_cast<uint16_t>(Profile::Game::DesignWidth),
                    static_cast<uint16_t>(Profile::Game::DesignHeight));

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
  bgfx::reset(static_cast<uint32_t>(Window->WindowWidth),
              static_cast<uint32_t>(Window->WindowHeight), BGFX_RESET_VSYNC);

  bgfx::setViewRect(DISPLAY_VIEW, 0, 0,
                    static_cast<uint16_t>(Window->WindowWidth),
                    static_cast<uint16_t>(Window->WindowHeight));

  bgfx::touch(DISPLAY_VIEW);
}

void Renderer::BeginFrame2D() {
  bgfx::setViewFrameBuffer(RENDER_VIEW, DrawFrameBuffer);

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
      .ShaderProgram = *SpriteShader,
  });

  bgfx::setIndexBuffer(BackBufferIndexBuffer);
  bgfx::setVertexBuffer(0, BackBufferVertexBuffer);

  SpriteShader->SubmitUniforms({}, {.Texture = DrawFrameBuffer.GetTexture()});

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

uint32_t Renderer::SubmitTexture(const TexFmt format,
                                 const std::span<const uint8_t> buffer,
                                 const int width, const int height) {
  static uint32_t curTextureId = 1;
  const uint32_t textureId = curTextureId++;

  Textures.emplace(textureId,
                   Texture(format, buffer, static_cast<size_t>(width),
                           static_cast<size_t>(height)));

  return textureId;
}

void Renderer::FreeTexture(const uint32_t id) {
  assert(Textures.contains(id));
  Textures.erase(id);
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

  bgfx::submit(RENDER_VIEW, CurrentState->ShaderProgram.get());
}

static bool ShouldFlip(const Sprite& sprite) {
  [[maybe_unused]] const RendererType rendererType =
      Impacto::Renderer->GetType();
  return sprite.Sheet.IsScreenCap && (false
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

void Renderer::SetState(const CommandBuffer& newState) {
  uint64_t stateFlags =
      BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_DEPTH_TEST_ALWAYS;
  bool differentStateFlags = !CurrentState.has_value();

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
      CurrentState->BlendMode != newState.BlendMode) {
    Flush();
    differentStateFlags = true;
  }
  switch (newState.BlendMode) {
    using enum RendererBlendMode;
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

  if (differentStateFlags) Flush();
  bgfx::setState(stateFlags);

  CurrentState = newState;
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
      .ShaderProgram = *SpriteShader,
      .Transformation = transformation,
  });

  SpriteShader->SubmitUniforms({},
                               {
                                   .Texture = Textures[sprite.Sheet.Texture],
                                   .ColorShift = colorShift,
                               });

  const RectF normalizedBounds = sprite.NormalizedBounds();

  constexpr static std::array<uint16_t, 6> indices = {0, 1, 3, 1, 2, 3};
  const std::array<VertexBufferSprites, 4> vertices = {
      VertexBufferSprites{
          .Position = dest.TopLeft,
          .UV = normalizedBounds.TopLeft(),
          .Tint = tints[0],
      },
      VertexBufferSprites{
          .Position = dest.BottomLeft,
          .UV = normalizedBounds.BottomLeft(),
          .Tint = tints[1],
      },
      VertexBufferSprites{
          .Position = dest.BottomRight,
          .UV = normalizedBounds.BottomRight(),
          .Tint = tints[2],
      },
      VertexBufferSprites{
          .Position = dest.TopRight,
          .UV = normalizedBounds.TopRight(),
          .Tint = tints[3],
      },
  };

  InsertVertices(indices, vertices, ShouldFlip(sprite));
}

}  // namespace Impacto::Bgfx
