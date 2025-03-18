#pragma once

#include "../button.h"
#include "../../../animation.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class TitleButton : public Widgets::Button {
 public:
  TitleButton(int id, Sprite const& norm, Sprite const& focused,
              Sprite const& highlight, glm::vec2 pos);
  void Render() override;
  void Update(float dt) override;
  void UpdateInput() override;
  void Hide() override;
  bool IsSubButton = false;
  Animation HighlightAnimation;
  Animation ChoiceBlinkAnimation;
  std::function<void(TitleButton*)> OnClickAnimCompleteHandler;
  bool PrevFocusState = false;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto