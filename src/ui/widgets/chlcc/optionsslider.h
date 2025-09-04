#pragma once

#include "optionsentry.h"
#include "../scrollbar.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class OptionsSlider : public OptionsEntry {
 public:
  OptionsSlider(float& value, float min, float max, const Sprite& bar,
                const Sprite& fill, glm::vec2 topRight, RectF hoverBounds,
                std::function<void(OptionsEntry*)> highlight);

  void Render() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;

  void Show() override;
  void Hide() override;

  void Move(glm::vec2 relativePos) override;

 private:
  float OldProgress;
  bool Changing = false;
  Animation ChangingFadeAnimation;

  Scrollbar Slider;
  Sprite BarSprite;
  Sprite FillSprite;
  float FillSpriteWidth;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
