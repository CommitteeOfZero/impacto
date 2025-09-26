#include "optionsslider.h"

#include "../../../profile/games/chlcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../inputsystem.h"
#include "../../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Vm::Interface;
using namespace Impacto::Profile::CHLCC::OptionsMenu;

OptionsSlider::OptionsSlider(float& value, const float min, const float max,
                             const Sprite bar, const Sprite fill,
                             const glm::vec2 topRight, const RectF hoverBounds,
                             const std::function<void(OptionsEntry*)> highlight,
                             const std::optional<Sprite> mutedSprite)
    : OptionsEntry(hoverBounds, highlight),
      OldProgress(value),
      Slider(0,
             topRight + SliderBarTopRightOffset + SliderBarFillOffset -
                 glm::vec2(bar.ScaledWidth(), 0.0f),
             min, max, &value, ScrollbarDirection::SBDIR_HORIZONTAL,
             fill.Bounds.GetSize()),
      BarSprite(bar),
      MutedSprite(mutedSprite),
      BeforeMutedProgress(min) {
  Slider.FillSprite = fill;

  ChangingFadeAnimation.SetDuration(SliderBarFadeDuration);
  ChangingFadeAnimation.LoopMode = AnimationLoopMode::ReverseDirection;
  ChangingFadeAnimation.StartIn();

  if (MutedSprite.has_value()) {
    const glm::vec2 clickAreaPos = Bounds.GetPos() + VoiceMutedOffset;
    const RectF clickRect(clickAreaPos.x, clickAreaPos.y,
                          MutedSprite->ScaledWidth(),
                          MutedSprite->ScaledHeight());
    const auto onClick = [this](const auto* clickArea) {
      if (*this->Slider.Value == this->Slider.StartValue) {
        *this->Slider.Value = this->BeforeMutedProgress;
      } else {
        this->BeforeMutedProgress = *this->Slider.Value;
        *this->Slider.Value = this->Slider.StartValue;
      }

      this->OldProgress = *this->Slider.Value;
      this->Changing = false;
    };

    MuteClickArea = ClickArea(0, clickRect, onClick);
  }
}

void OptionsSlider::Render() {
  Renderer->DrawSprite(BarSprite,
                       Slider.GetTrackBounds().GetPos() - SliderBarFillOffset);

  const float fillAlpha =
      Changing ? 0.5f + ChangingFadeAnimation.Progress * 0.5f : 1.0f;
  Slider.Tint.a = fillAlpha;
  Slider.Render();

  if (MutedSprite.has_value() && *Slider.Value <= Slider.StartValue) {
    const float mutedAlpha =
        (Changing && OldProgress > Slider.StartValue) ? fillAlpha : 1.0f;

    Renderer->DrawSprite(*MutedSprite, Bounds.GetPos() + VoiceMutedOffset,
                         {1.0f, 1.0f, 1.0f, mutedAlpha});
  }
}

void OptionsSlider::Update(float dt) {
  OptionsEntry::Update(dt);

  if (!HasFocus && Changing) {
    *Slider.Value = OldProgress;
    Changing = false;
  }

  ChangingFadeAnimation.Update(dt);
  Slider.Update(dt);
  if (MuteClickArea) MuteClickArea->Update(dt);
}

void OptionsSlider::UpdateInput(float dt) {
  static bool slidingByMouse = false;

  slidingByMouse |=
      (Slider.GetTrackBounds().ContainsPoint(Input::CurMousePos) &&
       Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) ||
      (Slider.GetTrackBounds().ContainsPoint(Input::CurTouchPos) &&
       Input::TouchWentDown[0]);
  slidingByMouse &=
      Input::MouseButtonIsDown[SDL_BUTTON_LEFT] || Input::TouchIsDown[0];

  OptionsEntry::UpdateInput(dt);

  Slider.HasFocus = HasFocus;
  Slider.UpdateInput(dt);
  Slider.ClampValue();

  if (MuteClickArea) MuteClickArea->UpdateInput(dt);

  Changing |= *Slider.Value != OldProgress;
  if (Changing && (PADinputButtonWentDown & PAD1A || slidingByMouse)) {
    UpdateValue();
  }
}

void OptionsSlider::UpdateValue() {
  OldProgress = *Slider.Value;
  Changing = false;
}

void OptionsSlider::Show() {
  OptionsEntry::Show();
  Slider.Show();
  if (MuteClickArea) MuteClickArea->Show();

  OldProgress = *Slider.Value;
}

void OptionsSlider::Hide() {
  Changing = false;
  *Slider.Value = OldProgress;

  Slider.Hide();
  if (MuteClickArea) MuteClickArea->Hide();
  OptionsEntry::Hide();
}

void OptionsSlider::Move(glm::vec2 relativePos) {
  OptionsEntry::Move(relativePos);
  Slider.Move(relativePos);
  if (MuteClickArea) MuteClickArea->Move(relativePos);
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
