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
  if (!CreateSDLWindow(windowFlags)) {
    Panic(LogChannel::General, "Failed to create SDL window: {:s}\n",
          SDL_GetError());
  }
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

void Window::Update() { UpdateDimensions(); }

void Window::SetDimensions(int width, int height, int msaa, float renderScale) {
  ImpLog(LogLevel::Info, LogChannel::General,
         "Attempting to change window dimensions to {:d} x {:d}, {:d}x MSAA, "
         "render scale {:f}\n",
         width, height, msaa, renderScale);
  assert(width > 0 && height > 0 && msaa >= 0 && renderScale > 0.0f);

  SDL_SetWindowSize(SDLWindow, width, height);

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

  DpiScale = SDL_GetWindowDisplayScale(SDLWindow);
}

}  // namespace Impacto::Bgfx
