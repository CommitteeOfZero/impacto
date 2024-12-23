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
                             float sliderSpeed,
                             std::function<void(OptionsEntry*)> select)
    : OptionsEntry(label, pos, highlightTint, select),
      BoxSprite(box),
      Slider(0, pos + SliderTrackOffset, 0.0f, 1.0f, &Progress,
             SBDIR_HORIZONTAL,
             glm::vec2(BoxSprite.ScaledWidth(), BoxSprite.ScaledHeight())) {
  Bounds.Width = SliderTrackOffset.x + BoxSprite.ScaledWidth();
  EntryButton.Bounds.Width = Bounds.Width;
}

OptionsSlider::OptionsSlider(const Sprite& box, const Sprite& label,
                             glm::vec2 pos, glm::vec4 highlightTint,
                             RectF sliderBounds, float sliderSpeed,
                             std::function<void(OptionsEntry*)> select)
    : OptionsEntry(label, pos, highlightTint, select),
      BoxSprite(box),
      Slider(0, sliderBounds.GetPos(), 0.0f, 1.0f, &Progress, SBDIR_HORIZONTAL,
             sliderBounds.GetSize()) {}

void OptionsSlider::Render() {
  OptionsEntry::Render();

  RectF highlightBounds(
      Bounds.X + SliderTrackOffset.x, Bounds.Y + SliderTrackOffset.y,
      Progress * BoxSprite.ScaledWidth(), BoxSprite.ScaledHeight());
  Renderer->DrawRect(highlightBounds, HighlightTint);

  Renderer->DrawSprite(BoxSprite, Bounds.GetPos() + SliderTrackOffset, Tint);
}

void OptionsSlider::UpdateInput() {
  OptionsEntry::UpdateInput();

  Slider.HasFocus = Selected;
  Slider.UpdateInput();
  Slider.ClampValue();
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto