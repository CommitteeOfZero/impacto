#include "mainmenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"
#include "../games/rne/mainmenu.h"
#include "../games/mo6tw/mainmenu.h"
#include "../../log.h"
#include "../../window.h"

namespace Impacto {
namespace Profile {
namespace MainMenu {

using namespace Impacto::MainMenu;

MainMenuType Type = MainMenuType::None;

Sprite MenuEntriesSprites[MenuEntriesNumMax];
Sprite MenuEntriesHSprites[MenuEntriesNumMax];

int MenuEntriesNum;
int MenuEntriesHNum;
float MenuEntriesX;
float MenuEntriesXOffset;
float MenuEntriesFirstY;
float MenuEntriesYPadding;

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window::Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

void Configure() {
  if (TryPushMember("MainMenu")) {
    AssertIs(kObjectType);

    Type = MainMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type == +MainMenuType::RNE) {
      Implementation = RNE::MainMenu::Configure();
    } else if (Type == +MainMenuType::MO6TW) {
      Implementation = MO6TW::MainMenu::Configure();
    }

    MenuEntriesNum = EnsureGetMemberInt("MenuEntriesNum");
    MenuEntriesHNum = EnsureGetMemberInt("MenuEntriesHNum");
    MenuEntriesX = EnsureGetMemberFloat("MenuEntriesX");
    MenuEntriesXOffset = EnsureGetMemberFloat("MenuEntriesXOffset");
    MenuEntriesFirstY = EnsureGetMemberFloat("MenuEntriesFirstY");
    MenuEntriesYPadding = EnsureGetMemberFloat("MenuEntriesYPadding");

    if (MenuEntriesNum > 0) {
      GetMemberSpriteArray(MenuEntriesSprites, MenuEntriesNum,
                           "MenuEntriesSprites");
    }
    if (MenuEntriesHNum > 0) {
      GetMemberSpriteArray(MenuEntriesHSprites, MenuEntriesHNum,
                           "MenuEntriesHighlightedSprites");
    }

    Pop();
  }
}

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto