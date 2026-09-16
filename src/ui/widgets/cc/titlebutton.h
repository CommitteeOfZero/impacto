#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CC {

class TitleButton : public Widgets::Button {
 public:
  TitleButton(int id, Sprite norm, Sprite focused,
              std::optional<Sprite> highlight, glm::vec2 pos)
      : Widgets::Button(id, norm, focused, highlight, pos) {}
  void Render() override;
  bool IsSubButton = false;
};

}  // namespace CC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto