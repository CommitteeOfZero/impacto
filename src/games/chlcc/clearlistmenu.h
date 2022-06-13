#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/label.h"
#include "../../profile/games/chlcc/clearlistmenu.h"

namespace Impacto{
namespace UI {
namespace CHLCC{

class ClearListMenu : public Menu {
 public:
  ClearListMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

 private:
  inline void DrawPlayTime(int totalSeconds);
  inline void DrawEndingCount();
  inline void DrawTIPSCount();
  inline void DrawEndingTree();

};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto