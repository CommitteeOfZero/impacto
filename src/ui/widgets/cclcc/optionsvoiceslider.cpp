#include "optionsvoiceslider.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"
#include "../../../audio/audiosystem.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Vm::Interface;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsVoiceSlider::OptionsVoiceSlider(
    float& volume, float min, float max, bool& muted, const Sprite& box,
    const Sprite& label, const Sprite& portrait, const Sprite& mutedPortrait,
    glm::vec2 pos, glm::vec4 highlightTint, float sliderSpeed,
    std::function<void(OptionsEntry*)> select,
    std::function<void(Widget*)> highlight)
    : OptionsSlider(
          volume, min, max, box, label, pos, highlightTint,
          RectF(pos.x + VoiceSliderOffset.x, pos.y + VoiceSliderOffset.y,
                box.ScaledWidth(), box.ScaledHeight()),
          sliderSpeed, select, highlight),
      Portrait(portrait),
      MutedPortrait(mutedPortrait),
      Muted(muted) {
  Bounds =
      RectF(Bounds.X, Bounds.Y, VoiceEntryDimensions.x, VoiceEntryDimensions.y);
  EntryButton.Bounds = Bounds;

  std::function<void(ClickArea*)> onClick = [this](auto* btn) {
    return MuteButtonOnClick(btn);
  };
  const RectF muteButtonBounds(Bounds.GetPos().x + PortraitOffset.x,
                               Bounds.GetPos().y + PortraitOffset.y,
                               portrait.ScaledWidth(), portrait.ScaledHeight());
  MuteButton = ClickArea(0, muteButtonBounds, onClick);
}

void OptionsVoiceSlider::Render() {
  HighlightTint.a = Tint.a;

  if (HasFocus) {
    RectF highlightBoundBox(Bounds.X, Bounds.Y, VoiceEntryDimensions.x,
                            VoiceEntryDimensions.y);
    Renderer->DrawQuad(highlightBoundBox, HighlightTint);
    Renderer->DrawQuad(
        RectF(highlightBoundBox.X + 2.0f, highlightBoundBox.Y + 2.0f,
              highlightBoundBox.Width - 4.0f, highlightBoundBox.Height - 4.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, Tint.a));
  }

  Renderer->DrawSprite(Muted ? MutedPortrait : Portrait,
                       Bounds.GetPos() + PortraitOffset, Tint);
  Renderer->DrawSprite(LabelSprite, Bounds.GetPos() + NametagOffset,
                       Selected ? Tint : glm::vec4(0.0f, 0.0f, 0.0f, Tint.a));

  RectF highlightBounds(Bounds.X + VoiceSliderOffset.x,
                        Bounds.Y + VoiceSliderOffset.y,
                        Slider.GetNormalizedValue() * BoxSprite.ScaledWidth(),
                        BoxSprite.ScaledHeight());
  Renderer->DrawQuad(highlightBounds, HighlightTint);
  Renderer->DrawSprite(BoxSprite, Bounds.GetPos() + VoiceSliderOffset, Tint);
}

void OptionsVoiceSlider::Update(float dt) {
  OptionsSlider::Update(dt);

  MuteButton.Update(dt);
}

void OptionsVoiceSlider::UpdateInput(float dt) {
  OptionsSlider::UpdateInput(dt);
  MuteButton.UpdateInput(dt);

  if (!HasFocus) return;

  if (PADinputButtonWentDown & PAD1Y) {
    Muted = !Muted;
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0.0f);
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

void OptionsVoiceSlider::Move(glm::vec2 relativePos) {
  OptionsSlider::Move(relativePos);
  MuteButton.Move(relativePos);
}

void OptionsVoiceSlider::MuteButtonOnClick(ClickArea* target) {
  if (HasFocus) Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0.0f);

  Muted = !Muted;
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto