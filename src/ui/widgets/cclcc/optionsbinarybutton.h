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
                      std::function<void(OptionsEntry*)> select);

  void Render() override;
  void UpdateInput() override;

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