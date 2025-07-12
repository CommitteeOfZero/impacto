#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "librarysubmenus.h"
#include "musicmenu.h"
#include "moviemenu.h"
#include "albummenu.h"

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
  LibrarySubmenu& GetMenuFromType(LibraryMenuPageType menuType) const;
  LibraryMenuPageType GetMenuTypeFromButton(Widget* btn) const;

  Widgets::Group MainItems;

  LibraryMenuPageType CurrentLibraryMenu = LibraryMenuPageType::Album;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto