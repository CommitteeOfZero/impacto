#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace SGHD {

class TitleButton : public Widgets::Button {
 public:
  TitleButton(int id, Sprite const& norm, Sprite const& focused,
              Sprite const& highlight, glm::vec2 pos)
      : Widgets::Button(id, norm, focused, highlight, pos) {}
  void Render() override;
  bool IsSubButton = false;
};

}  // namespace SGHD
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
