#pragma once

#include "./optionsslider.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsVoiceSlider : public OptionsSlider {
 public:
  OptionsVoiceSlider(float& volume, bool& muted, const Sprite& box,
                     const Sprite& label, const Sprite& portrait,
                     const Sprite& mutedPortrait, glm::vec2 pos,
                     glm::vec4 highlightTint, float sliderSpeed,
                     std::function<void(OptionsEntry*)> select,
                     std::function<void(Widget*)> highlight);
  void Render() override;
  void Update(float dt) override;
  void UpdateInput() override;

  void Show() override;
  void Hide() override;

  void Move(glm::vec2 relativePos) override;
  void MoveTo(glm::vec2 pos) override;

 private:
  const Sprite& Portrait;
  const Sprite& MutedPortrait;

  bool& Muted;
  ClickArea MuteButton;
  void MuteButtonOnClick(ClickArea* target);
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto