#include "mainmenu.h"
#include "../../profile_internal.h"
#include "../../../window.h"
#include "../../../log.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace MainMenu {

Sprite MainMenuBackgroundSprite;
Sprite MenuEntriesHighlightedSprite;

float MenuEntriesTargetWidth;
float MainMenuX;
float MainMenuY;

Impacto::MainMenu::MainMenuBase* Configure() {
  Impacto::MO6TW::MainMenu* result = new Impacto::MO6TW::MainMenu();

  MenuEntriesHighlightedSprite =
      EnsureGetMemberSprite("MenuEntriesHighlightedSprite");
  MainMenuBackgroundSprite = EnsureGetMemberSprite("MainMenuBackgroundSprite");
  MenuEntriesTargetWidth = EnsureGetMemberFloat("MenuEntriesTargetWidth");
  MainMenuX = EnsureGetMemberFloat("MainMenuX");
  MainMenuY = EnsureGetMemberFloat("MainMenuY");

  return result;
}

}  // namespace MainMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
