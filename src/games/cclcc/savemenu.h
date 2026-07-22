#pragma once

#include <optional>
#include "commonmenu.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/savemenu.h"
#include "../../profile/games/cclcc/savemenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class SaveMenu : public UI::SaveMenu, public CommonMenu {
 public:
  SaveMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

  void RefreshCurrentEntryInfo() override;

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  int PrevPage = 0;
  int CurrentPage = 0;
  std::optional<int> LastFocusedEntry;
  Widgets::Group* MainItems[Profile::CCLCC::SaveMenu::Pages]{};
  Animation FadeAnimation;
  Animation PageAnimation;
  bool HasCleared = true;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto