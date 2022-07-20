#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../data/savesystem.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class SaveMenu : public Menu {
 public:
  SaveMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  inline void DrawCircles();
  inline void DrawErin();
  inline void DrawRedBar();
  inline void DrawTitles();

  std::vector<Widgets::Group*> FullSavePages;
  std::vector<Widgets::Group*> QuickSavePages;
  std::vector<Widgets::Group*>* SavePages;
  Widgets::Group* MainItems;
  Animation MenuTransition;
  Animation TitleFade;
  SaveSystem::SaveType EntryType;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto