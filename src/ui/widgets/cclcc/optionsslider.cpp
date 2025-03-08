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

OptionsSlider::OptionsSlider(float& value, float min, float max,
                             const Sprite& box, const Sprite& label,
                             glm::vec2 pos, glm::vec4 highlightTint,
                             float sliderSpeed,
                             std::function<void(OptionsEntry*)> select,
                             std::function<void(Widget*)> highlight)
    : OptionsEntry(label, pos, highlightTint, select, highlight),
      Progress(value),
      BoxSprite(box),
      Slider(0, pos + SliderTrackOffset, min, max, &value, SBDIR_HORIZONTAL,
             glm::vec2(BoxSprite.ScaledWidth(), BoxSprite.ScaledHeight())) {
  Bounds.Width = SliderTrackOffset.x + BoxSprite.ScaledWidth();
  EntryButton.Bounds.Width = Bounds.Width;
}

OptionsSlider::OptionsSlider(float& value, float min, float max,
                             const Sprite& box, const Sprite& label,
                             glm::vec2 pos, glm::vec4 highlightTint,
                             RectF sliderBounds, float sliderSpeed,
                             std::function<void(OptionsEntry*)> select,
                             std::function<void(Widget*)> highlight)
    : OptionsEntry(label, pos, highlightTint, select, highlight),
      Progress(value),
      BoxSprite(box),
      Slider(0, sliderBounds.GetPos(), min, max, &value, SBDIR_HORIZONTAL,
             sliderBounds.GetSize()) {}

void OptionsSlider::Render() {
  OptionsEntry::Render();

  RectF highlightBounds(Bounds.X + SliderTrackOffset.x,
                        Bounds.Y + SliderTrackOffset.y,
                        Slider.GetNormalizedValue() * BoxSprite.ScaledWidth(),
                        BoxSprite.ScaledHeight());
  Renderer->DrawRect(highlightBounds, HighlightTint);

  Renderer->DrawSprite(BoxSprite, Bounds.GetPos() + SliderTrackOffset, Tint);
}

void OptionsSlider::Update(float dt) {
  OptionsEntry::Update(dt);

  Slider.Update(dt);
}

void OptionsSlider::UpdateInput() {
  OptionsEntry::UpdateInput();

  Slider.HasFocus = Selected;
  Slider.UpdateInput();
  Slider.ClampValue();
}

void OptionsSlider::Move(glm::vec2 relativePos) {
  OptionsEntry::Move(relativePos);
  Slider.Move(relativePos);
}

void OptionsSlider::MoveTo(glm::vec2 pos) {
  const glm::vec2 relativePosition = pos - Bounds.GetPos();
  OptionsEntry::MoveTo(pos);
  Slider.Move(relativePosition);
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto