#pragma once

#include "../../ui/menu.h"
#include "../../spriteanimation.h"
#include "../../ui/widgets/group.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class OptionsMenu : public Menu {
 public:
  OptionsMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

 private:
  void GoToPage(int pageNumber);

  Animation FadeAnimation;
  SpriteAnimation PoleAnimation;

  Widgets::Group* BasicPage;
  Widgets::Group* TextPage;
  Widgets::Group* SoundPage;
  Widgets::Group* VoicePage;

  int CurrentPage;
  std::vector<UI::Widgets::Group*> Pages;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto