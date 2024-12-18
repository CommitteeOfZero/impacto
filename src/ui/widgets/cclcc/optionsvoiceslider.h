#pragma once

#include "./optionsslider.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsVoiceSlider : public OptionsSlider {
 public:
  OptionsVoiceSlider(const Sprite& box, const Sprite& label,
                     const Sprite& portrait, const Sprite& mutedPortrait,
                     glm::vec2 pos, glm::vec4 highlightTint, float sliderSpeed,
                     std::function<void(OptionsEntry*)> select);
  void Render() override;
  void UpdateInput() override;

 private:
  const Sprite& Portrait;
  const Sprite& MutedPortrait;

  bool Muted = false;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto