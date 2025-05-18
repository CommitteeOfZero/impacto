#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"
#include "../../profile/games/cclcc/librarymenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class LibraryMenu : public Menu {
 public:
  LibraryMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  Animation FadeAnimation;

  void LibraryMenuButtonOnClick(Widgets::Button* target);

 private:
  using LibraryMenuPageType = Profile::CCLCC::LibraryMenu::LibraryMenuPageType;
  Widgets::Group MainItems;
  LibraryMenuPageType CurrentLibraryMenu = +LibraryMenuPageType::Album;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto