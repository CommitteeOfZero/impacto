#include "window.h"

#include <SDL3/SDL_vulkan.h>

#include "../renderer.h"
#include "../../log.h"
#include "../../profile/game.h"
#include "../../profile/profile.h"
#include "../../userconfig.h"
#include "../../game.h"

#include "renderer.h"

namespace Impacto {
namespace Vulkan {

void VulkanWindow::UpdateDimensions() {
  WindowDimensionsChanged = false;
  SDL_GetWindowSizeInPixels(SDLWindow, &WindowWidth, &WindowHeight);
  if (WindowWidth != lastWidth || WindowHeight != lastHeight ||
      MsaaCount != lastMsaa || RenderScale != lastRenderScale) {
    WindowDimensionsChanged = true;
    ImpLog(LogLevel::Debug, LogChannel::General,
           "Drawable size (pixels): {:d} x {:d} ({:d}x MSAA requested, render "
           "scale {:f})\n",
           WindowWidth, WindowHeight, MsaaCount, RenderScale);
  }
  lastWidth = WindowWidth;
  lastHeight = WindowHeight;
  lastMsaa = MsaaCount;
  lastRenderScale = RenderScale;

  DpiScale = SDL_GetWindowDisplayScale(SDLWindow);
  // SDL_SetWindowInputFocus(SDLWindow);
  MainRendererInstance->RecreateSwapChain();
}

void VulkanWindow::Init() {
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

  SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;
  CreateSDLWindow(windowFlags);
}

void VulkanWindow::SetDimensions(int width, int height, int msaa,
                                 float renderScale) {}

void VulkanWindow::SwapRTs() {}

void VulkanWindow::Update() {}

void VulkanWindow::Draw() {
#ifndef IMPACTO_DISABLE_IMGUI
  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }
#endif
}

void VulkanWindow::Shutdown() {
  SDL_DestroyWindow(SDLWindow);
  SDL_Quit();
  // TODO move exit to users
  exit(0);
}

}  // namespace Vulkan
}  // namespace Impacto