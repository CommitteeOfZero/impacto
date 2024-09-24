#pragma once

#include "../../ui/backlogmenu.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace CC {

class BacklogMenu : public UI::BacklogMenu {
 public:
  BacklogMenu();

  void Render();
  void MenuButtonOnClick(Widgets::BacklogEntry* target);

 private:
  const Sprite* MaskSprite;
};

}  // namespace CC
}  // namespace UI
}  // namespace Impacto