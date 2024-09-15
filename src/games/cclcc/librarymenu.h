#pragma once

#include "../../ui/menu.h"

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
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto