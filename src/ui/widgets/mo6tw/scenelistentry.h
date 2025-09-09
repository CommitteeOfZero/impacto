#pragma once

#include "../button.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

class SceneListEntry : public Widgets::Button {
 public:
  SceneListEntry(int id, Widgets::Label* number, Widgets::Label* lockedText,
                 Widgets::Label* unlockedText, Sprite const& highlight,
                 bool isLocked);
  void Render() override;

  using Widget::Move;
  void Move(glm::vec2 relativePosition) override;

  bool IsLocked = false;

 private:
  Widgets::Label* Number;
  Widgets::Label* LockedText;
  Widgets::Label* UnlockedText;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto