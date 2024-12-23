
#include <functional>
#include <stdexcept>
#include <utility>
#include <variant>
#include <SDL.h>
#include "../util.h"
#include "../log.h"
#include "window.h"

namespace Impacto {
#define VARIANT_VISIT_CALL(RETURN_TYPE, WINDOW_MEMBER)                        \
  std::visit(Overloaded{[](std::monostate) -> RETURN_TYPE {                   \
                          ImpLog(LL_Fatal, LC_Render,                         \
                                 "Window Used in an Invalid State");          \
                          exit(1);                                            \
                        },                                                    \
                        [&](auto&& window) { return window.WINDOW_MEMBER; }}, \
             Impl);

void WindowInterface::Init() { return VARIANT_VISIT_CALL(void, Init()); };

void WindowInterface::SetDimensions(int width, int height, int msaa,
                                    float renderScale) {
  return VARIANT_VISIT_CALL(void,
                            SetDimensions(width, height, msaa, renderScale));
}
// Aspect ratio corrected viewport in window coordinates
RectF WindowInterface::GetViewport() {
  return VARIANT_VISIT_CALL(RectF, GetViewport());
};

// Aspect ratio corrected viewport in window coordinates scaled by RenderScale
RectF WindowInterface::GetScaledViewport() {
  return VARIANT_VISIT_CALL(RectF, GetScaledViewport());
}
void WindowInterface::SwapRTs() { return VARIANT_VISIT_CALL(void, SwapRTs()); }

void WindowInterface::Update() { return VARIANT_VISIT_CALL(void, Update()); }

void WindowInterface::Draw() { return VARIANT_VISIT_CALL(void, Draw()); }
void WindowInterface::Shutdown() {
  return VARIANT_VISIT_CALL(void, Shutdown());
}

void WindowInterface::UpdateDimensions() {
  return VARIANT_VISIT_CALL(void, UpdateDimensions());
};

int WindowInterface::WindowHeight() const {
  return VARIANT_VISIT_CALL(int, WindowHeight);
};
int WindowInterface::WindowWidth() const {
  return VARIANT_VISIT_CALL(int, WindowWidth);
};
int WindowInterface::MsaaCount() const {
  return VARIANT_VISIT_CALL(int, MsaaCount);
};
float WindowInterface::RenderScale() const {
  return VARIANT_VISIT_CALL(float, RenderScale);
};
float WindowInterface::DpiScaleX() const {
  return VARIANT_VISIT_CALL(float, DpiScaleX);
};
float WindowInterface::DpiScaleY() const {
  return VARIANT_VISIT_CALL(float, DpiScaleY);
};
SDL_Window* WindowInterface::SDLWindow() const {
  return VARIANT_VISIT_CALL(SDL_Window*, SDLWindow);
};
bool WindowInterface::WindowDimensionsChanged() const {
  return VARIANT_VISIT_CALL(bool, WindowDimensionsChanged);
}

}  // namespace Impacto