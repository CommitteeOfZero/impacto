#include "optionsslider.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsSlider::OptionsSlider(const Sprite& box, const Sprite& label,
                             glm::vec2 pos, glm::vec4 highlightTint)
    : OptionsEntry(label, pos, highlightTint), BoxSprite(box) {
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

void OptionsSlider::UpdateInput() { OptionsEntry::UpdateInput(); }

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto