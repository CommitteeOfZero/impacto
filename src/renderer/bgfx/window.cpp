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

void Window::Draw() {
#ifndef IMPACTO_DISABLE_IMGUI
  ImGui::Render();
  ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
#endif

  bgfx::frame();
}

}  // namespace Impacto::Bgfx
