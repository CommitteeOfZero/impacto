#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class AlbumMenu : public Menu {
 public:
  AlbumMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  void CharacterButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;
  Widgets::Group* ImageGrid;
  Widgets::Group* SecondaryItems;

  int SelectedCharacterId = -1;

  Animation FadeAnimation;

  void SwitchToCharacter(int id);
  void LoadCharacter(int id);
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto