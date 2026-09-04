#include "renderer.h"

#include "../../log.h"
#include "../../userconfig.h"

#include <bgfx/platform.h>
#include <imgui_impl_bgfx.h>
#include <glm/gtc/type_ptr.hpp>

namespace Impacto::Bgfx {

constexpr bgfx::ViewId RENDER_VIEW = 0;   // Uses render dimensions
constexpr bgfx::ViewId DISPLAY_VIEW = 1;  // Uses viewport dimensions
constexpr bgfx::ViewId IMGUI_VIEW = 255;

Renderer::Renderer() {
  bgfx::Init initStruct{};

  initStruct.type = []() -> bgfx::RendererType::Enum {
    switch (UserConfig::AdvancedSettings.ActiveRenderer) {
#ifndef IMPACTO_DISABLE_OPENGL
      case RendererType::OpenGL:
        return bgfx::RendererType::OpenGL;
      case RendererType::OpenGLES:
        return bgfx::RendererType::OpenGLES;
#endif
#ifndef IMPACTO_DISABLE_VULKAN
      case RendererType::Vulkan:
        return bgfx::RendererType::Vulkan;
#endif
#ifndef IMPACTO_DISABLE_DIRECT3D
      case RendererType::Direct3D:
        return bgfx::RendererType::Direct3D12;
#endif
#ifndef IMPACTO_DISABLE_METAL
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
    const std::string errorMsg = fmt::format(
        "Unsupported video driver \"{:s}\"", SDL_GetCurrentVideoDriver());
    ImpLog(LogLevel::Fatal, LogChannel::Render, "{:s}", errorMsg);
    throw std::runtime_error(errorMsg);
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
    constexpr const char* errorMsg = "Failed to initialize BGFX";
    ImpLog(LogLevel::Fatal, LogChannel::Render, errorMsg);
    throw std::runtime_error(errorMsg);
  }

  BackBufferFrameBuffer = FrameBuffer::CreateBackBufferFrameBuffer();

  ViewMatrix = glm::mat4(1.0f);
  BackBufferProjectionMatrix = glm::ortho(
      0.0f, static_cast<float>(UserConfig::CommonSettings.ResolutionWidth),
      static_cast<float>(UserConfig::CommonSettings.ResolutionHeight), 0.0f);

  Indices.resize(100);
  IndexBuffer = bgfx::createDynamicIndexBuffer(
      static_cast<uint32_t>(Indices.size()), BGFX_BUFFER_ALLOW_RESIZE);
  if (!bgfx::isValid(IndexBuffer)) {
    constexpr const char* errorMsg = "Failed to create index buffer";
    ImpLog(LogLevel::Fatal, LogChannel::Render, errorMsg);
    throw std::runtime_error(errorMsg);
  }

  VertexBufferSpritesLayout.begin()
      .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float, true)
      .add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float)
      .end();

  Vertices.resize(100);
  VertexBuffer = bgfx::createDynamicVertexBuffer(
      bgfx::copy(
          Vertices.data(),
          static_cast<uint32_t>(Vertices.size() * sizeof(VertexBufferSprites))),
      VertexBufferSpritesLayout);
  if (!bgfx::isValid(VertexBuffer)) {
    constexpr const char* errorMsg = "Failed to create vertex buffer";
    ImpLog(LogLevel::Fatal, LogChannel::Render, errorMsg);
    throw std::runtime_error(errorMsg);
  }

  ShaderPrograms[ShaderProgramType::Sprite] =
      ShaderProgram::Create("vs_sprite", "fs_sprite");

  ImGui_Implbgfx_Init(IMGUI_VIEW);
  switch (UserConfig::AdvancedSettings.ActiveRenderer) {
#ifndef IMPACTO_DISABLE_OPENGL
    case RendererType::OpenGL:
    case RendererType::OpenGLES:
      ImGui_ImplSDL3_InitForOpenGL(Window->SDLWindow, nullptr);
      break;
#endif
#ifndef IMPACTO_DISABLE_VULKAN
    case RendererType::Vulkan:
      ImGui_ImplSDL3_InitForVulkan(Window->SDLWindow);
      break;
#endif
#ifndef IMPACTO_DISABLE_DIRECT3D
    case RendererType::Direct3D:
      ImGui_ImplSDL3_InitForD3D(Window->SDLWindow);
      break;
#endif
#ifndef IMPACTO_DISABLE_METAL
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
  DrawFrameBuffer =
      FrameBuffer(static_cast<uint16_t>(Profile::Game::DesignWidth),
                  static_cast<uint16_t>(Profile::Game::DesignHeight));

  ProjectionMatrix =
      glm::ortho(0.0f, Profile::Game::DesignWidth, Profile::Game::DesignHeight,
                 0.0f, -Profile::Game::DesignWidth, Profile::Game::DesignWidth);
}

void Renderer::BeginFrame() {
  bgfx::reset(static_cast<uint32_t>(Window->WindowWidth),
              static_cast<uint32_t>(Window->WindowHeight), BGFX_RESET_VSYNC);

  constexpr uint32_t black = 0xff000000;
  bgfx::setViewClear(DISPLAY_VIEW, BGFX_CLEAR_COLOR, black);
  bgfx::setViewRect(DISPLAY_VIEW, 0, 0,
                    static_cast<uint16_t>(Window->WindowWidth),
                    static_cast<uint16_t>(Window->WindowHeight));
  bgfx::setViewTransform(DISPLAY_VIEW, glm::value_ptr(ViewMatrix),
                         glm::value_ptr(BackBufferProjectionMatrix));
  bgfx::setViewFrameBuffer(DISPLAY_VIEW, BackBufferFrameBuffer);

  bgfx::touch(DISPLAY_VIEW);
}

void Renderer::BeginFrame2D() {
  constexpr uint32_t transparentWhite = 0x00FFFFFF;
  bgfx::setViewClear(RENDER_VIEW,
                     BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL,
                     transparentWhite);
  bgfx::setViewRect(RENDER_VIEW, 0, 0,
                    static_cast<uint16_t>(Profile::Game::DesignWidth),
                    static_cast<uint16_t>(Profile::Game::DesignHeight));
  bgfx::setViewTransform(RENDER_VIEW, glm::value_ptr(ViewMatrix),
                         glm::value_ptr(ProjectionMatrix));
  bgfx::setViewFrameBuffer(RENDER_VIEW, DrawFrameBuffer);

  bgfx::touch(RENDER_VIEW);
}

void Renderer::EndFrame() {}

#ifndef IMPACTO_DISABLE_IMGUI
void Renderer::ImGuiBeginFrame() {
  ImGui_Implbgfx_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  bgfx::setViewClear(IMGUI_VIEW, BGFX_CLEAR_COLOR, 0);
  bgfx::touch(IMGUI_VIEW);
}
#endif

void Renderer::Shutdown() {
  bgfx::destroy(IndexBuffer);
  bgfx::destroy(VertexBuffer);

#ifndef IMPACTO_DISABLE_IMGUI
  ImGui_ImplSDL3_Shutdown();
  ImGui_Implbgfx_Shutdown();
  ImGui::DestroyContext();
#endif

  bgfx::shutdown();
}

}  // namespace Impacto::Bgfx
