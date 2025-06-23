#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "librarysubmenus.h"
#include "musicmenu.h"
#include "moviemenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class LibraryMenu : public Menu {
 public:
  LibraryMenu();

  void Init() override;
  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;

  Animation FadeAnimation;
  Animation ButtonBlinkAnimation;

  void LibraryMenuButtonOnClick(Widgets::Button* target);

 private:
  using LibraryMenuPageType = Profile::CCLCC::LibraryMenu::LibraryMenuPageType;
  LibrarySubmenu& GetMenuFromType(LibraryMenuPageType menuType);

  Widgets::Group MainItems;
  std::optional<LibraryMenuPageType> CurrentLibraryMenu;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto