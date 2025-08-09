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

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;

 private:
  void DrawCircles();
  void DrawErin();
  void DrawRedBar();

  void DrawPlayTime(float yOffset);
  void DrawEndingCount(float yOffset);
  void DrawTIPSCount(float yOffset);
  void DrawAlbumCompletion(float yOffset);
  void DrawEndingTree(float yOffset);

  void DrawButtonPrompt();

  void UpdateTitles();

  Animation MenuTransition;
  Animation TitleFade;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto