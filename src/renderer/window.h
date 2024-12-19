#pragma once

#include <functional>
#include <stdexcept>
#include <utility>
#include <variant>
#include <SDL.h>
#include "opengl/window.h"
#include "dx9/window.h"
#include "vulkan/window.h"

namespace Impacto {
class WindowInterface {
 public:
  WindowInterface() = default;

  template <typename T>
  explicit WindowInterface(T&& window) {
    Impl.emplace(std::in_place, std::forward<T>(window));
  }

  template <typename T>
  void Emplace() {
    Impl.emplace<T>();
  }

  void Init();

  void SetDimensions(int width, int height, int msaa, float renderScale);
  // Aspect ratio corrected viewport in window coordinates
  RectF GetViewport();

  // Aspect ratio corrected viewport in window coordinates scaled by RenderScale
  RectF GetScaledViewport();
  void SwapRTs();
  void Update();
  void Draw();
  void Shutdown();

  int WindowWidth() const;
  int WindowHeight() const;
  SDL_Window* SDLWindow() const;
  int MsaaCount() const;
  float DpiScaleX() const;
  float DpiScaleY() const;
  float RenderScale() const;
  bool WindowDimensionsChanged() const;

  template <typename T>
  auto& GetImpl() const {
    return std::get<T>(Impl);
  }

  template <typename T>
  auto& GetImpl() {
    return std::get<T>(Impl);
  }

 protected:
  void UpdateDimensions();

  std::variant<std::monostate, OpenGL::GLWindow, Vulkan::VulkanWindow,
               DirectX9::DirectX9Window>
      Impl;
};

}  // namespace Impacto