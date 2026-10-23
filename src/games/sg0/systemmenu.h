#pragma once

#include "../../ui/menu.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 system menu (menuchip.dds): the game's screen capture inside a
// monitor frame and a ring of icon buttons. _Startup_win drives it with
// SystemMenu(0), SW_SYSMENUCT counting 0..96 in steps of 3 and SystemMenu(1)
// each frame; the chosen button id goes to SW_SYSMENUCNO, which the script
// dispatches with a 12-entry jump table (0 Backlog, 1 Quick Save, 2 Save,
// 3 Load, 4 Quick Load, 5 TIPS, 6 Config, 7 Help, 8 Title, 9 Full Screen,
// 10 Exit Game, 11 Close Menu).
class SystemMenu : public Menu {
 public:
  SystemMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;
  void Init() override;

  Sprite ScreenCap;

 private:
  int Cursor = 0;  // index into the navigation order
  bool OpenedAsDirect = false;
  int LastSysMenuCt = 0;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
