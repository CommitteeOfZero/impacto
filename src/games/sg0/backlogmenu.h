#pragma once

#include "../../ui/backlogmenu.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 backlog (backlog.dds). _Startup_win opens it with
// SW_SYSSUBMENUNO 1, sets SW_SYSSUBMENUCT to 32 at once and fades
// SW_SYSSUBMENUALPHA 0..256 in steps of 16 (and back down when closing), so
// visibility and fade follow the alpha.
class BacklogMenu : public UI::BacklogMenu<Widgets::BacklogEntry> {
 public:
  void Hide() override;
  void Render() override;

 protected:
  void UpdateVisibility() override;

 private:
  int LastAlpha = 0;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
