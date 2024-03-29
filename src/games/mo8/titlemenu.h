#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/group.h"

namespace Impacto {
namespace UI {
namespace MO8 {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void MenuButtonOnClick(Widgets::Button* target);
  void ContinueButtonOnClick(Widgets::Button* target);
  void GalleryButtonOnClick(Widgets::Button* target);

  Animation PrimaryFadeAnimation;

  Animation MainItemsHideAnimation;
  Animation ContinueItemsShowAnimation;
  Animation GalleryItemsShowAnimation;

 private:
  Widgets::Group* MainItems;

  Widgets::Button* NewGame;
  Widgets::Button* Continue;
  Widgets::Button* Options;
  Widgets::Button* Gallery;

  Widgets::Group* ContinueItems;

  Widgets::Button* Load;
  Widgets::Button* QuickLoad;

  Widgets::Group* GalleryItems;

  Widgets::Button* AlbumButton;
  Widgets::Button* MusicButton;
  Widgets::Button* ClearListButton;
  Widgets::Button* WarningButton;
  Widgets::Button* AdditionalButton;
  Widgets::Button* DLCButton;

  bool ContinueSelected = false;
  bool GallerySelected = false;

  void UpdateSubMenu(Animation* showAnimation, Widgets::Group* subMenuGroup,
                     bool* subMenuSelected);
};

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto