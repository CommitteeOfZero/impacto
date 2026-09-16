#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

class TitleButton : public Widgets::Button {
 public:
  TitleButton(int id, Sprite norm, Sprite focused, glm::vec2 pos)
      : Widgets::Button(id, norm, focused, std::nullopt, pos) {}
  void Render() override;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto