#pragma once

#include "../../ui/menu.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 EXTRA > CLEAR LIST (clearlist.dds): six Nixie tubes, one per
// ending, lit with the ending's character and title once its clear flag
// (801-806, set by CLRFLG.scx) is on. _Startup_win shows it with
// SW_SYSSUBMENUNO 7 and SW_SYSSUBMENUCT counting 0..64, and closes it itself
// on Back (script key 6 or a right click).
class ClearListMenu : public Menu {
 public:
  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;

 private:
  float Alpha() const;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
