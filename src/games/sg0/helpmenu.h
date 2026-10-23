#pragma once

#include "../../ui/menu.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 help: the operating-instruction pages from manual.mpk.
// _Startup_win opens it with SW_SYSSUBMENUNO 11 and SW_SYSSUBMENUCT counting
// to 32, then calls Help(1) every frame until flag 1830 (closed) is set.
class HelpMenu : public Menu {
 public:
  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

 private:
  int Page = 0;
  int LastSubMenuCount = 0;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
