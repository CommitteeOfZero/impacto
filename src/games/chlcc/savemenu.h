#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../data/savesystem.h"
#include "../../ui/widgets/chlcc/saveentrybutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::UI::Widgets::CHLCC;

class SaveMenu : public Menu {
 public:
  SaveMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  void UpdateEntry(SaveEntryButton* saveEntryButton);
  inline void DrawCircles();
  inline void DrawErin();
  inline void DrawRedBar();
  inline void DrawTitles();
  inline void DrawPageNumber(float yOffset);
  inline void DrawButtonPrompt();
  inline void DrawSelectData(float yOffset);

  int CurrentFullSavePage = 0;
  int CurrentQuickSavePage = 0;
  int* CurrentPage;

  std::vector<Widgets::Group*> FullSavePages;
  std::vector<Widgets::Group*> QuickSavePages;
  std::vector<Widgets::Group*>* SavePages;
  Widgets::Group* MainItems;
  Animation MenuTransition;
  Animation TitleFade;
  Animation SelectDataTextFade;
  SaveSystem::SaveType EntryType;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto