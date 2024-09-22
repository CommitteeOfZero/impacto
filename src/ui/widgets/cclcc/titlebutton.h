#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class TitleButton : public Widgets::Button {
 public:
  TitleButton(int id, Sprite const& norm, Sprite const& focused,
              Sprite const& highlight, glm::vec2 pos)
      : Widgets::Button(id, norm, focused, highlight, pos) {
    HighlightAnimation.DurationIn = 0.3f;
    HighlightAnimation.DurationOut = 0.3f;
  }
  void Render() override;
  void UpdateInput() override;
  bool IsSubButton = false;
  Animation HighlightAnimation;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto