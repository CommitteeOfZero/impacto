#include "window.h"

#include <SDL_vulkan.h>

#include "../renderer.h"
#include "../../log.h"
#include "../../profile/game.h"
#include "../../game.h"

#include "renderer.h"

namespace Impacto {
namespace Vulkan {

void VulkanWindow::UpdateDimensions() {
  WindowDimensionsChanged = false;
  SDL_Vulkan_GetDrawableSize(SDLWindow, &WindowWidth, &WindowHeight);
  if (WindowWidth != lastWidth || WindowHeight != lastHeight ||
      MsaaCount != lastMsaa || RenderScale != lastRenderScale) {
    WindowDimensionsChanged = true;
    ImpLog(LL_Debug, LC_General,
           "Drawable size (pixels): %d x %d (%dx MSAA requested, render scale "
           "%f)\n",
           WindowWidth, WindowHeight, MsaaCount, RenderScale);
  }
  lastWidth = WindowWidth;
  lastHeight = WindowHeight;
  lastMsaa = MsaaCount;
  lastRenderScale = RenderScale;

  int osWindowWidth, osWindowHeight;
  SDL_GetWindowSize(SDLWindow, &osWindowWidth, &osWindowHeight);
  DpiScaleX = (float)WindowWidth / (float)osWindowWidth;
  DpiScaleY = (float)WindowHeight / (float)osWindowHeight;
  SDL_SetWindowInputFocus(SDLWindow);
  MainRendererInstance->RecreateSwapChain();
}

RectF VulkanWindow::GetViewport() {
  RectF viewport;
  float scale = fmin((float)WindowWidth / Profile::DesignWidth,
                     (float)WindowHeight / Profile::DesignHeight);
  viewport.Width = Profile::DesignWidth * scale;
  viewport.Height = Profile::DesignHeight * scale;
  viewport.X = ((float)WindowWidth - viewport.Width) / 2.0f;
  viewport.Y = ((float)WindowHeight - viewport.Height) / 2.0f;
  return viewport;
}

RectF VulkanWindow::GetScaledViewport() {
  RectF viewport = GetViewport();
  viewport.Width *= RenderScale;
  viewport.Height *= RenderScale;
  viewport.X *= RenderScale;
  viewport.Y *= RenderScale;
  return viewport;
}

void VulkanWindow::Init() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_General, "Creating window\n");
  IsInit = true;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
    ImpLog(LL_Fatal, LC_General, "SDL initialisation failed: %s\n",
           SDL_GetError());
    Shutdown();
    return;
  }

  SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");

  uint32_t windowFlags = SDL_WINDOW_VULKAN;
#if IMPACTO_USE_SDL_HIGHDPI
  windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
#endif

  SDLWindow = SDL_CreateWindow(Profile::WindowName, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, 1280, 720, windowFlags);

  if (SDLWindow == NULL) {
    ImpLog(LL_Error, LC_General, SDL_GetError());
    ImpLog(LL_Error, LC_General, "Window creation failed: %s\n",
           SDL_GetError());
    return;
  }

  SDL_GetWindowSize(SDLWindow, &WindowWidth, &WindowHeight);
  ImpLog(LL_Debug, LC_General, "Window size (screen coords): %d x %d\n",
         WindowWidth, WindowHeight);
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