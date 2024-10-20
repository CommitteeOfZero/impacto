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
    : BoxSprite(box), LabelSprite(label), HighlightTint(highlightTint) {
  Bounds = RectF(pos.x, pos.y, SliderTrackOffset.x + box.ScaledWidth(),
                 LabelSprite.ScaledHeight());
}

void OptionsSlider::Render() {
  HighlightTint.a = Tint.a;

  Renderer->DrawSprite(LabelSprite, Bounds.GetPos(),
                       {0.0f, 0.0f, 0.0f, Tint.a});

  RectF highlightBounds(
      Bounds.X + SliderTrackOffset.x, Bounds.Y + SliderTrackOffset.y,
      Progress * BoxSprite.ScaledWidth(), BoxSprite.ScaledHeight());
  Renderer->DrawRect(highlightBounds, HighlightTint);

  Renderer->DrawSprite(BoxSprite, Bounds.GetPos() + SliderTrackOffset, Tint);
}

void OptionsSlider::UpdateInput() {}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto