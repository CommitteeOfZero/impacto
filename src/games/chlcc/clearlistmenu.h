#pragma once

#include "commonmenu.h"
#include "../../ui/menu.h"
#include "../../profile/games/chlcc/clearlistmenu.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class ClearListMenu : public Menu, public CommonMenu {
 public:
  ClearListMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;

 private:
  void DrawPlayTime(float yOffset);
  void DrawEndingCount(float yOffset);
  void DrawTIPSCount(float yOffset);
  void DrawAlbumCompletion(float yOffset);
  void DrawEndingTree(float yOffset);
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto