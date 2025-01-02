#pragma once

#include "./optionsentry.h"
#include "../clickbutton.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsBinaryButton : public OptionsEntry {
 public:
  OptionsBinaryButton(const Sprite& box, const Sprite& trueLabel,
                      const Sprite& falseLabel, const Sprite& label,
                      glm::vec2 pos, glm::vec4 highlightTint,
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
  ClickButton TrueButton;
  ClickButton FalseButton;

  const Sprite& TrueSprite;
  const Sprite& FalseSprite;

  void TrueOnClick(ClickButton* target);
  void FalseOnClick(ClickButton* target);

  const Sprite& BoxSprite;

  bool State = true;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto