#pragma once

#include "./optionsentry.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsSlider : public OptionsEntry {
 public:
  OptionsSlider(const Sprite& box, const Sprite& label, glm::vec2 pos,
                glm::vec4 highlightTint, float sliderSpeed);

  void Render() override;
  void Update(float dt) override;

 private:
  const Sprite& BoxSprite;

  float Progress = 0.0f;
  float SliderSpeed = 1.0f;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto