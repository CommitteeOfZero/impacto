#pragma once

#include "commonmenu.h"
#include "../../ui/savemenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../data/savesystem.h"
#include "../../ui/widgets/chlcc/saveentrybutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::UI::Widgets::CHLCC;

class SaveMenu : public UI::SaveMenu, public CommonMenu {
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
  void DrawPageNumber(float yOffset);

  int CurrentFullSavePage = 0;
  int CurrentQuickSavePage = 0;
  int* CurrentPage;
  int PrevPage = 0;

  std::vector<Widgets::Group*> FullSavePages;
  std::vector<Widgets::Group*> QuickSavePages;
  std::vector<Widgets::Group*>* SavePages;
  SaveSystem::SaveType EntryType;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto