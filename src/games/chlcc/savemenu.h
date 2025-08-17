#pragma once

#include "../../ui/menu.h"
#include "../../ui/savemenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../data/savesystem.h"
#include "../../ui/widgets/chlcc/saveentrybutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::UI::Widgets::CHLCC;

class SaveMenu : public UI::SaveMenu {
 public:
  SaveMenu();

  void Init() override;
  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

  void RefreshCurrentEntryInfo() override;

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawPageNumber(float yOffset);
  void DrawButtonPrompt();
  void DrawSelectData(float yOffset);

  void UpdateTitles();

  int CurrentFullSavePage = 0;
  int CurrentQuickSavePage = 0;
  int* CurrentPage;
  int PrevPage = 0;

  std::vector<Widgets::Group*> FullSavePages;
  std::vector<Widgets::Group*> QuickSavePages;
  std::vector<Widgets::Group*>* SavePages;
  Widgets::Group* MainItems;
  Animation MenuTransition;
  Animation TitleFade;
  Animation SelectDataTextFade;
  SaveSystem::SaveType EntryType;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto