#include "systemmenuentrybutton.h"

#include "../../../profile/games/chlcc/systemmenu.h"

#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"
#include "../../../inputsystem.h"
namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Profile::CHLCC::SystemMenu;

SystemMenuEntryButton::SystemMenuEntryButton(int id, Sprite const& norm,
                                             Sprite const& focused,
                                             glm::vec4 focusTint,
                                             Sprite const& highlight,
                                             glm::vec2 pos, RectF hoverBounds)
    : Button(id, norm, focused, highlight, pos, hoverBounds),
      FocusTint(focusTint) {
  HighlightOffset = glm::vec2(3.0f, 3.0f);
  StarAnimation.SetDuration(StarAnimationDuration);
}

void SystemMenuEntryButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(
        NormalSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        RgbIntToFloat(0x28537f));
  }

  glm::vec4 tint = IsLocked   ? RgbIntToFloat(0x808080)
                   : HasFocus ? FocusTint
                              : glm::vec4(1.0f);
  Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), tint);
  if (!StarAnimation.IsPlaying()) return;

  const glm::vec2 starAnchor = glm::vec2(Bounds.X, Bounds.Y + Bounds.Height);

  const float rotationAngle =
      StarAnimationDuration * StarAnimation.Progress * StarRotationSpeed -
      LeftAngle;

  const glm::vec4 starOpacity =
      glm::vec4({1.0, 1.0, 1.0, 1.0f - StarAnimation.Progress});

  for (int i = 0; i < STAR_COUNT; i++) {
    const glm::vec2 currentPos =
        starAnchor + glm::mix(StarsOffsetsStart[i], StarsOffsetsEnd[i],
                              StarAnimation.Progress);
    // left stars rotate clockwise, right stars rotate counter-clockwise
    const float currentAngle =
        i < STAR_COUNT / 2 ? rotationAngle : -rotationAngle;
    const CornersQuad dest = StarSprite.ScaledBounds()
                                 .RotateAroundCenter(currentAngle)
                                 .Translate(currentPos);

    Renderer->DrawSprite(StarSprite, dest, starOpacity);
  }
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

void SystemMenuEntryButton::Update(float dt) {
  Button::Update(dt);
  StarAnimation.Update(dt);
}

void SystemMenuEntryButton::UpdateInput(float dt) {
  if (Enabled) {
    constexpr float rotationAngle = -15.0f * std::numbers::pi_v<float> / 180.0f;
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
      if (IsLocked) return;
      // reset animation only if it's not playing already
      StarAnimation.StartIn(StarAnimation.State != AnimationState::Playing);
    }
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto