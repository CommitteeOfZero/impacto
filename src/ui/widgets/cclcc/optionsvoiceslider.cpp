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

OptionsVoiceSlider::OptionsVoiceSlider(const Sprite& box, const Sprite& label,
                                       const Sprite& portrait,
                                       const Sprite& mutedPortrait,
                                       glm::vec2 pos, glm::vec4 highlightTint,
                                       float sliderSpeed)
    : OptionsSlider(box, label, pos, highlightTint, sliderSpeed),
      Portrait(portrait),
      MutedPortrait(mutedPortrait) {}

void OptionsVoiceSlider::Render() {
  HighlightTint.a = Tint.a;

  Renderer->DrawSprite(Muted ? MutedPortrait : Portrait, Bounds.GetPos(), Tint);
  Renderer->DrawSprite(LabelSprite, Bounds.GetPos() + NametagOffset,
                       Selected ? Tint : glm::vec4(0.0f, 0.0f, 0.0f, Tint.a));

  RectF highlightBounds(
      Bounds.X + VoiceSliderOffset.x, Bounds.Y + VoiceSliderOffset.y,
      Progress * BoxSprite.ScaledWidth(), BoxSprite.ScaledHeight());
  Renderer->DrawRect(highlightBounds, HighlightTint);
  Renderer->DrawSprite(BoxSprite, Bounds.GetPos() + VoiceSliderOffset, Tint);
}

void OptionsVoiceSlider::UpdateInput() {
  OptionsEntry::UpdateInput();
  if (!Selected) return;

  Muted ^= (bool)(PADinputButtonWentDown & PAD1Y);
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto