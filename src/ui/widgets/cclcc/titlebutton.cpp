#include "titlebutton.h"

#include "../../../renderer/renderer.h"
#include "../../../inputsystem.h"
#include "../../../profile/games/cclcc/titlemenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::TitleMenu;

TitleButton::TitleButton(int id, Sprite const& norm, Sprite const& focused,
                         Sprite const& highlight, glm::vec2 pos)
    : Widgets::Button(id, norm, focused, highlight, pos) {
  HighlightAnimation.DurationIn = HighlightAnimationDurationIn;
  HighlightAnimation.DurationOut = HighlightAnimationDurationOut;
  ChoiceBlinkAnimation.DurationIn = ChoiceBlinkAnimationDurationIn;
  ChoiceBlinkAnimation.DurationOut = 0;
}

void TitleButton::UpdateInput(float dt) {
  if (Enabled &&
      (IsSubButton || HighlightAnimation.State == AnimationState::Stopped) &&
      ChoiceBlinkAnimation.IsOut()) {
    Button::UpdateInput(dt);
  }
}

void TitleButton::Update(float dt) {
  Widget::Update(dt);
  HighlightAnimation.Update(dt);
  ChoiceBlinkAnimation.Update(dt);
  if (ChoiceBlinkAnimation.IsIn()) {
    ChoiceBlinkAnimation.Progress = 0.0f;
    if (OnClickAnimCompleteHandler) {
      OnClickAnimCompleteHandler(this);
    }
  }
  if (PrevFocusState != HasFocus) {
    PrevFocusState = HasFocus;
    if (Input::CurrentInputDevice != Input::Device::Mouse) {
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
    }
    if (!IsSubButton) {
      if (HighlightAnimation.IsOut() && HasFocus) {
        HighlightAnimation.StartIn();
      } else if (HighlightAnimation.IsIn() && !HasFocus) {
        HighlightAnimation.StartOut();
      }
    }
  }
}

void TitleButton::Hide() {
  Button::Hide();
  HighlightAnimation.Progress = 0.0f;
  ChoiceBlinkAnimation.Progress = 0.0f;
  PrevFocusState = false;
}

void TitleButton::Render() {
  // Calculate the blink effect to occur 4 times during the animation
  float blinkProgress = ChoiceBlinkAnimation.Progress * 4.0f;
  float blinkAlpha = 0.5f * (1.0f + cos(blinkProgress * glm::two_pi<float>()));
  glm::vec4 BlinkTint = glm::vec4(1.0f, 1.0f, 1.0f, Tint.a);
  if (ChoiceBlinkAnimation.State == AnimationState::Playing) {
    BlinkTint.a = blinkAlpha;
  }
  if (HasFocus ||
      (!IsSubButton && HighlightAnimation.State == AnimationState::Playing) ||
      ChoiceBlinkAnimation.State == AnimationState::Playing) {
    if (!IsSubButton) {  // Main buttons
      Sprite newHighlightSprite = HighlightSprite;
      float smoothProgress =
          HighlightAnimation.State == AnimationState::Playing
              ? glm::smoothstep(0.0f, 1.0f, HighlightAnimation.Progress)
              : 1.0f;

      newHighlightSprite.Bounds.Width *= smoothProgress;
      Renderer->DrawSprite(newHighlightSprite,
                           glm::vec2(Bounds.X - ItemHighlightOffsetX,
                                     Bounds.Y - ItemHighlightOffsetY),
                           BlinkTint);
      glm::vec4 pointerTint =
          glm::vec4(1.0f, 1.0f, 1.0f, smoothProgress * blinkAlpha);
      Renderer->DrawSprite(
          ItemHighlightPointerSprite,
          glm::vec2(Bounds.X - ItemHighlightPointerY, Bounds.Y), pointerTint);
      Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    } else {  // Sub buttons
      Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                           BlinkTint);
      Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y),
                           BlinkTint);
    }
  } else {
    Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  }
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto