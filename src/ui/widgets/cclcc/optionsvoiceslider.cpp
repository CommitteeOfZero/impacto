#include "optionsvoiceslider.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Vm::Interface;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsVoiceSlider::OptionsVoiceSlider(
    const Sprite& box, const Sprite& label, const Sprite& portrait,
    const Sprite& mutedPortrait, glm::vec2 pos, glm::vec4 highlightTint,
    float sliderSpeed, std::function<void(OptionsEntry*)> select)
    : OptionsSlider(
          box, label, pos, highlightTint,
          RectF(pos.x + VoiceSliderOffset.x, pos.y + VoiceSliderOffset.y,
                box.ScaledWidth(), box.ScaledHeight()),
          sliderSpeed, select),
      Portrait(portrait),
      MutedPortrait(mutedPortrait) {
  Bounds =
      RectF(Bounds.X, Bounds.Y, VoiceEntryDimensions.x, VoiceEntryDimensions.y);
  EntryButton.Bounds = Bounds;

  std::function<void(ClickButton*)> onClick = std::bind(
      &OptionsVoiceSlider::MuteButtonOnClick, this, std::placeholders::_1);
  const RectF muteButtonBounds(Bounds.GetPos().x + PortraitOffset.x,
                               Bounds.GetPos().y + PortraitOffset.y,
                               portrait.ScaledWidth(), portrait.ScaledHeight());
  MuteButton = ClickButton(0, muteButtonBounds, onClick);
}

void OptionsVoiceSlider::Render() {
  HighlightTint.a = Tint.a;

  if (HasFocus || EntryButton.Hovered) {
    RectF highlightBoundBox(Bounds.X, Bounds.Y, VoiceEntryDimensions.x,
                            VoiceEntryDimensions.y);
    Renderer->DrawRect(highlightBoundBox, HighlightTint);
    Renderer->DrawRect(highlightBoundBox + RectF(2.0f, 2.0f, -4.0f, -4.0f),
                       glm::vec4(1.0f, 1.0f, 1.0f, Tint.a));
  }

  Renderer->DrawSprite(Muted ? MutedPortrait : Portrait,
                       Bounds.GetPos() + PortraitOffset, Tint);
  Renderer->DrawSprite(LabelSprite, Bounds.GetPos() + NametagOffset,
                       Selected ? Tint : glm::vec4(0.0f, 0.0f, 0.0f, Tint.a));

  RectF highlightBounds(
      Bounds.X + VoiceSliderOffset.x, Bounds.Y + VoiceSliderOffset.y,
      Progress * BoxSprite.ScaledWidth(), BoxSprite.ScaledHeight());
  Renderer->DrawRect(highlightBounds, HighlightTint);
  Renderer->DrawSprite(BoxSprite, Bounds.GetPos() + VoiceSliderOffset, Tint);
}

void OptionsVoiceSlider::UpdateInput() {
  OptionsSlider::UpdateInput();
  MuteButton.UpdateInput();

  if (!HasFocus) return;

  if (PADinputButtonWentDown & PAD1Y) {
    Muted = !Muted;
    Audio::Channels[Audio::AC_REV]->Play("sysse", 2, false, 0.0f);
  }

  if ((bool)(PADinputButtonWentDown & (PAD1LEFT | PAD1RIGHT)) &&
      !(bool)(PADinputButtonWentDown & (PAD1UP | PAD1DOWN))) {
    Audio::Channels[Audio::AC_REV]->Play("sysse", 1, false, 0.0f);
  }
}

void OptionsVoiceSlider::Show() {
  OptionsSlider::Show();
  MuteButton.Show();
}

void OptionsVoiceSlider::Hide() {
  OptionsSlider::Hide();
  MuteButton.Hide();
}

void OptionsVoiceSlider::MuteButtonOnClick(ClickButton* target) {
  if (HasFocus) Audio::Channels[Audio::AC_REV]->Play("sysse", 2, false, 0.0f);

  Muted = !Muted;
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto