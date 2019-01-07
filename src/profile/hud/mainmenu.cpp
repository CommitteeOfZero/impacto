#include "mainmenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"
#include "../games/rne/mainmenu.h"
#include "../../log.h"
#include "../../window.h"

namespace Impacto {
namespace Profile {
namespace MainMenu {

using namespace Impacto::MainMenu;

MainMenuType Type = MainMenuType::None;

Sprite ButtonBackgroundSprite;
Sprite ButtonPromptsSprite;
Sprite MenuEntriesSprites[MenuEntriesNum];
Sprite MenuEntriesHSprites[MenuEntriesNum];
float ButtonBackgroundStartX;
float ButtonBackgroundX;
float ButtonBackgroundY;
float ButtonBackgroundTargetWidth;
float ButtonBackgroundSprStartX;
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
    }

    ButtonBackgroundSprite = EnsureGetMemberSprite("ButtonBackgroundSprite");
    ButtonPromptsSprite = EnsureGetMemberSprite("ButtonPromptsSprite");
    ButtonBackgroundStartX = EnsureGetMemberFloat("ButtonBackgroundStartX");
    ButtonBackgroundX = EnsureGetMemberFloat("ButtonBackgroundX");
    ButtonBackgroundY = EnsureGetMemberFloat("ButtonBackgroundY");
    ButtonBackgroundTargetWidth =
        EnsureGetMemberFloat("ButtonBackgroundTargetWidth");
    ButtonBackgroundSprStartX =
        EnsureGetMemberFloat("ButtonBackgroundSprStartX");
    MenuEntriesX = EnsureGetMemberFloat("MenuEntriesX");
    MenuEntriesXOffset = EnsureGetMemberFloat("MenuEntriesXOffset");
    MenuEntriesFirstY = EnsureGetMemberFloat("MenuEntriesFirstY");
    MenuEntriesYPadding = EnsureGetMemberFloat("MenuEntriesYPadding");

    GetMemberSpriteArray(MenuEntriesSprites, MenuEntriesNum,
                         "MenuEntriesSprites");
    GetMemberSpriteArray(MenuEntriesHSprites, MenuEntriesNum,
                         "MenuEntriesHighlightedSprites");
    Pop();
  }
}

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto