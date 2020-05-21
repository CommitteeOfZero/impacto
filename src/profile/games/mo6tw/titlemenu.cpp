#include "titlemenu.h"
#include "../../../log.h"
#include "../../../window.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TitleMenu {

Sprite BackgroundSprite;
Sprite CopyrightSprite;
Sprite LogoSprite;
Sprite MenuBackgroundSprite;
Sprite MenuItemLockedSprite;
Sprite MenuItemLockedSpriteH;

float MenuEntriesX;
float MenuEntriesFirstY;
float MenuEntriesYPadding;
float MenuEntriesTargetWidth;
float LogoX;
float LogoY;
float CopyrightX;
float CopyrightY;

Impacto::TitleMenu::TitleMenuBase* Configure() {
  Impacto::MO6TW::TitleMenu* result = new Impacto::MO6TW::TitleMenu();

  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  CopyrightSprite = EnsureGetMemberSprite("CopyrightSprite");
  LogoSprite = EnsureGetMemberSprite("LogoSprite");
  LogoX = EnsureGetMemberFloat("LogoX");
  LogoY = EnsureGetMemberFloat("LogoY");
  CopyrightX = EnsureGetMemberFloat("CopyrightX");
  CopyrightY = EnsureGetMemberFloat("CopyrightY");
  MenuBackgroundSprite = EnsureGetMemberSprite("MenuBackgroundSprite");
  MenuItemLockedSprite = EnsureGetMemberSprite("MenuItemLockedSprite");
  MenuItemLockedSpriteH = EnsureGetMemberSprite("MenuItemLockedSpriteH");
  MenuEntriesTargetWidth = EnsureGetMemberFloat("MenuEntriesTargetWidth");
  MenuEntriesX = EnsureGetMemberFloat("MenuEntriesX");
  MenuEntriesFirstY = EnsureGetMemberFloat("MenuEntriesFirstY");
  MenuEntriesYPadding = EnsureGetMemberFloat("MenuEntriesYPadding");

  return result;
}

}  // namespace TitleMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto