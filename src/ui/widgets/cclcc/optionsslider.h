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
  OptionsSlider(const Sprite& box, const Sprite& label, glm::vec2 pos,
                glm::vec4 highlightTint, float sliderSpeed,
                std::function<void(OptionsEntry*)> select);

  void Render() override;
  void Update(float dt) override;
  void UpdateInput() override;

  void Move(glm::vec2 relativePos) override;
  void MoveTo(glm::vec2 pos) override;

 protected:
  OptionsSlider(const Sprite& box, const Sprite& label, glm::vec2 pos,
                glm::vec4 highlightTint, RectF sliderBounds, float sliderSpeed,
                std::function<void(OptionsEntry*)> select);

  const Sprite& BoxSprite;

  Scrollbar Slider;
  float Progress = 0.0f;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto