#include "window.h"

#include "../../log.h"
#include "../../userconfig.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui.h>
#include <imgui_impl_bgfx.h>
#endif  // IMPACTO_DISABLE_IMGUI

namespace Impacto::Bgfx {

void Window::Init() {
  ImpLog(LogLevel::Info, LogChannel::General, "Creating bgfx window\n");

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    std::string errorMsg =
        fmt::format("SDL initialization failed: {:s}\n", SDL_GetError());
    ImpLog(LogLevel::Fatal, LogChannel::General, "{:s}", errorMsg);
    throw std::runtime_error(errorMsg);
  }

#ifdef SDL_PLATFORM_ANDROID
  SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif

  SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");

  uint32_t windowFlags = 0;
#if IMPACTO_USE_SDL_HIGHDPI
  windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
#endif

  CreateSDLWindow(windowFlags);
}

void Window::Shutdown() {
  SDL_DestroyWindow(SDLWindow);
  SDL_Quit();
  // TODO: move exit to users
  exit(0);
}

void Window::Draw() {
#ifndef IMPACTO_DISABLE_IMGUI
  ImGui::Render();
  ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
#endif

  bgfx::frame();
}

RectF Window::GetViewport() {
  RectF viewport;
  const float designWidth =
      Profile::Game::HasInit ? Profile::Game::DesignWidth : WindowWidth;
  const float designHeight =
      Profile::Game::HasInit ? Profile::Game::DesignHeight : WindowHeight;

  const float scale = std::min(static_cast<float>(WindowWidth) / designWidth,
                               static_cast<float>(WindowHeight) / designHeight);
  viewport.Width = designWidth * scale;
  viewport.Height = designHeight * scale;
  viewport.X = (WindowWidth - viewport.Width) / 2.0f;
  viewport.Y = (WindowHeight - viewport.Height) / 2.0f;

  return viewport;
}

void Window::Update() { UpdateDimensions(); }

void Window::SetDimensions(int width, int height, int msaa, float renderScale) {
  ImpLog(LogLevel::Info, LogChannel::General,
         "Attempting to change window dimensions to {:d} x {:d}, {:d}x MSAA, "
         "render scale {:f}\n",
         width, height, msaa, renderScale);
  assert(width > 0 && height > 0 && msaa >= 0 && renderScale > 0.0f);

  SDL_SetWindowSize(SDLWindow, static_cast<int>(width / DpiScaleX),
                    static_cast<int>(height / DpiScaleY));

  MsaaCount = msaa;
  RenderScale = renderScale;
}

void Window::UpdateDimensions() {
  WindowDimensionsChanged = false;

  SDL_GetWindowSizeInPixels(SDLWindow, &WindowWidth, &WindowHeight);
  if (WindowWidth != lastWidth || WindowHeight != lastHeight) {
    WindowDimensionsChanged = true;
    ImpLog(LogLevel::Debug, LogChannel::General,
           "Drawable size (pixels): {:d} x {:d}\n", WindowWidth, WindowHeight);
  }
  lastWidth = WindowWidth;
  lastHeight = WindowHeight;

  int osWindowWidth, osWindowHeight;
  SDL_GetWindowSize(SDLWindow, &osWindowWidth, &osWindowHeight);
  DpiScaleX = static_cast<float>(WindowWidth) / osWindowWidth;
  DpiScaleY = static_cast<float>(WindowHeight) / osWindowHeight;
}

RectF Window::GetScaledViewport() {
  return GetViewport().Scale(glm::vec2(RenderScale), {0.0f, 0.0f});
}

}  // namespace Impacto::Bgfx
