#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"

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
  Widgets::Group* MainItems;
  // Widgets::CCLCC::TitleButton* Album;
  // Widgets::CCLCC::TitleButton* Sound;
  // Widgets::CCLCC::TitleButton* Movie;

  int CurrentLibraryMenu = 0;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto