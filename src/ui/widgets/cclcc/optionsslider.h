#pragma once

#include "./optionsentry.h"
#include "../scrollbar.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsSlider : public OptionsEntry {
 public:
  OptionsSlider(float& value, float min, float max, const Sprite& box,
                const Sprite& label, glm::vec2 pos, glm::vec4 highlightTint,
                float sliderSpeed, std::function<void(OptionsEntry*)> select,
                std::function<void(Widget*)> highlight);

  void Render() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;

  void Move(glm::vec2 relativePos) override;

 protected:
  OptionsSlider(float& value, float min, float max, const Sprite& box,
                const Sprite& label, glm::vec2 pos, glm::vec4 highlightTint,
                RectF sliderBounds, float sliderSpeed,
                std::function<void(OptionsEntry*)> select,
                std::function<void(Widget*)> highlight);

  const Sprite& BoxSprite;

  Scrollbar Slider;
  float& Progress;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto