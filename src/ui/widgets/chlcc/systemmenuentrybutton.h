#pragma once

#include "../button.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class SystemMenuEntryButton : public Button {
 public:
  SystemMenuEntryButton(int id, Sprite const& norm, Sprite const& focused,
                        glm::vec4 focusTint, Sprite const& highlight,
                        glm::vec2 pos);
  void Render() override;

 protected:
  glm::vec4 FocusTint;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto