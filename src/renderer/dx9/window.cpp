#include "window.h"

#include "../renderer.h"

#include "../../log.h"

#include "../../profile/game.h"
#include "../../profile/profile.h"
#include "../../userconfig.h"

#include "../../game.h"

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui_impl_dx9.h>
#endif

namespace Impacto {
namespace DirectX9 {

void DirectX9Window::UpdateDimensions() {
  WindowDimensionsChanged = false;
  // SDL_Vulkan_GetDrawableSize(SDLWindow, &WindowWidth, &WindowHeight);
  if (WindowWidth != lastWidth || WindowHeight != lastHeight ||
      MsaaCount != lastMsaa || RenderScale != lastRenderScale) {
    WindowDimensionsChanged = true;
    ImpLog(LogLevel::Debug, LogChannel::General,
           "Drawable size (pixels): {:d} x {:d} ({:d}x MSAA requested, render "
           "scale "
           "{:f})\n",
           WindowWidth, WindowHeight, MsaaCount, RenderScale);
  }
  lastWidth = WindowWidth;
  lastHeight = WindowHeight;
  lastMsaa = MsaaCount;
  lastRenderScale = RenderScale;

  DpiScale = SDL_GetWindowDisplayScale(SDLWindow);
  // SDL_SetWindowInputFocus(SDLWindow);
}

void DirectX9Window::Init() {
  assert(IsInit == false);
  ImpLog(LogLevel::Info, LogChannel::General, "Creating window\n");
  IsInit = true;

#ifdef __ANDROID__
  SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    ImpLog(LogLevel::Fatal, LogChannel::General,
           "SDL initialisation failed: {:s}\n", SDL_GetError());
    Shutdown();
    return;
  }

  SDL_WindowFlags windowFlags = 0;
  CreateSDLWindow(windowFlags);
}

void DirectX9Window::SetDimensions(int width, int height, int msaa,
                                   float renderScale) {}

void DirectX9Window::SwapRTs() {}

void DirectX9Window::Update() {}

void DirectX9Window::Draw() {
#ifndef IMPACTO_DISABLE_IMGUI
  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }
#endif
}

void DirectX9Window::Shutdown() {
  SDL_DestroyWindow(SDLWindow);
  SDL_Quit();
  // TODO move exit to users
  exit(0);
}

}  // namespace DirectX9
}  // namespace Impacto