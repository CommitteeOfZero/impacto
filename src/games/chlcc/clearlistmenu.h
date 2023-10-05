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

  inline void DrawPlayTime(float yOffset);
  inline void DrawEndingCount(float yOffset);
  inline void DrawTIPSCount(float yOffset);
  inline void DrawAlbumCompletion(float yOffset);
  inline void DrawEndingTree(float yOffset);

  inline void DrawButtonPrompt();

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