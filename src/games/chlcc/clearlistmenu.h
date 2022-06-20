#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/label.h"
#include "../../profile/games/chlcc/clearlistmenu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class ClearListMenu : public Menu {
 public:
  ClearListMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

 private:
  inline void DrawCircles();
  inline void DrawErin();
  inline void DrawRedBar();

  inline void DrawPlayTime(int yOffset);
  inline void DrawEndingCount(int yOffset);
  inline void DrawTIPSCount(int yOffset);
  inline void DrawAlbumCompletion(int yOffset);
  inline void DrawEndingTree(int yOffset);

  Animation MenuTransition;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto