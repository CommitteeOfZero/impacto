#include "optionsslider.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Vm::Interface;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsSlider::OptionsSlider(const Sprite& box, const Sprite& label,
                             glm::vec2 pos, glm::vec4 highlightTint,
                             float sliderSpeed)
    : OptionsEntry(label, pos, highlightTint),
      BoxSprite(box),
      SliderSpeed(sliderSpeed) {
  Bounds.Width = SliderTrackOffset.x + BoxSprite.ScaledWidth();
}

void OptionsSlider::Render() {
  OptionsEntry::Render();

  RectF highlightBounds(
      Bounds.X + SliderTrackOffset.x, Bounds.Y + SliderTrackOffset.y,
      Progress * BoxSprite.ScaledWidth(), BoxSprite.ScaledHeight());
  Renderer->DrawRect(highlightBounds, HighlightTint);

  Renderer->DrawSprite(BoxSprite, Bounds.GetPos() + SliderTrackOffset, Tint);
}

void OptionsSlider::Update(float dt) {
  OptionsEntry::Update(dt);
  if (!Selected) return;

  int slideDirection = (bool)(PADinputButtonIsDown & PAD1RIGHT) -
                       (bool)(PADinputButtonIsDown & PAD1LEFT);
  Progress =
      std::clamp(Progress + slideDirection * SliderSpeed * dt, 0.0f, 1.0f);
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto