#include "systemmenuentrybutton.h"
#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"
#include "../../../inputsystem.h"
namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

SystemMenuEntryButton::SystemMenuEntryButton(int id, Sprite const& norm,
                                             Sprite const& focused,
                                             glm::vec4 focusTint,
                                             Sprite const& highlight,
                                             glm::vec2 pos, RectF hoverBounds)
    : Button(id, norm, focused, highlight, pos, hoverBounds),
      FocusTint(focusTint) {
  HighlightOffset = glm::vec2(3.0f, 3.0f);
}

void SystemMenuEntryButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(
        NormalSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        RgbIntToFloat(0x28537f));
  }
  Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y),
                       HasFocus ? FocusTint : glm::vec4(1.0f));
}
glm::vec2 SystemMenuEntryButton::RotatePoint(const glm::vec2& point,
                                             const glm::vec2& center,
                                             float angleRadians) {
  float s = sin(-angleRadians);
  float c = cos(-angleRadians);

  glm::vec2 translated = point - center;

  float xnew = translated.x * c - translated.y * s;
  float ynew = translated.x * s + translated.y * c;

  return glm::vec2{xnew, ynew} + center;
}
void SystemMenuEntryButton::UpdateInput(float dt) {
  if (Enabled) {
    float rotationAngle = -0.2618f;
    const RectF& bounds = (HoverBounds != RectF{}) ? HoverBounds : Bounds;
    glm::vec2 center = bounds.Center();
    glm::vec2 rotatedPos;
    if (Input::CurrentInputDevice == Input::Device::Mouse &&
        Input::PrevMousePos != Input::CurMousePos) {
      rotatedPos = RotatePoint(Input::CurMousePos, center, rotationAngle);
      Hovered = bounds.ContainsPoint(rotatedPos);
    } else if (Input::CurrentInputDevice == Input::Device::Touch &&
               Input::TouchIsDown[0] &&
               Input::PrevTouchPos != Input::CurTouchPos) {
      rotatedPos = RotatePoint(Input::CurTouchPos, center, rotationAngle);
      Hovered = bounds.ContainsPoint(rotatedPos);
    }
    if (OnClickHandler && HasFocus &&
        ((Hovered &&
          Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A) ||
         (Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1A))) {
      OnClickHandler(this);
    }
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto