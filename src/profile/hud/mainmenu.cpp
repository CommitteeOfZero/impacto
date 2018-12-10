#include "mainmenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"
#include "../../log.h"
#include "../../window.h"

namespace Impacto {
namespace Profile {
namespace MainMenu {

using namespace Impacto::MainMenu;

MainMenuType Type = MainMenuType::None;

Sprite ButtonBackgroundSprite;
Sprite SkyBackgroundSprite;
Sprite SkyArrowSprite;
Sprite SkyTextSprite;
Sprite ButtonPromptsSprite;
Sprite MenuEntriesSprites[MenuEntriesNum];
Sprite MenuEntriesHSprites[MenuEntriesNum];
float SkyBackgroundBeginX;
float SkyBackgroundY;
float SkyTextBeginX;
float SkyTextY;
float ButtonBackgroundStartX;
float ButtonBackgroundX;
float ButtonBackgroundY;
float ButtonBackgroundTargetWidth;
float ButtonBackgroundSprStartX;
float MenuEntriesX;
float MenuEntriesXSkew;
float MenuEntriesXOffset;
float MenuEntriesFirstY;
float MenuEntriesYPadding;
float MenuEntriesTargetWidth;
float SkyInStartProgress;
float SkyOutStartProgress;
float SkyMoveDurationIn;
float SkyMoveDurationOut;
float EntriesMoveDurationIn;
float EntriesMoveDurationOut;
float HighlightDurationIn;
float HighlightDurationOut;

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
      EnsurePushMemberOfType("Background", kObjectType);
      BackgroundAnimation = RNE::ParseTileBackground();
      Pop();

      ButtonBackgroundSprite = EnsureGetMemberSprite("ButtonBackgroundSprite");
      SkyBackgroundSprite = EnsureGetMemberSprite("SkyBackgroundSprite");
      SkyArrowSprite = EnsureGetMemberSprite("SkyArrowSprite");
      SkyTextSprite = EnsureGetMemberSprite("SkyTextSprite");
      SkyBackgroundBeginX = EnsureGetMemberFloat("SkyBackgroundBeginX");
      SkyBackgroundY = EnsureGetMemberFloat("SkyBackgroundY");
      ButtonPromptsSprite = EnsureGetMemberSprite("ButtonPromptsSprite");
      SkyTextBeginX = EnsureGetMemberFloat("SkyTextBeginX");
      SkyTextY = EnsureGetMemberFloat("SkyTextY");
      ButtonBackgroundStartX = EnsureGetMemberFloat("ButtonBackgroundStartX");
      ButtonBackgroundX = EnsureGetMemberFloat("ButtonBackgroundX");
      ButtonBackgroundY = EnsureGetMemberFloat("ButtonBackgroundY");
      ButtonBackgroundTargetWidth =
          EnsureGetMemberFloat("ButtonBackgroundTargetWidth");
      ButtonBackgroundSprStartX =
          EnsureGetMemberFloat("ButtonBackgroundSprStartX");
      MenuEntriesX = EnsureGetMemberFloat("MenuEntriesX");
      MenuEntriesXSkew = EnsureGetMemberFloat("MenuEntriesXSkew");
      MenuEntriesXOffset = EnsureGetMemberFloat("MenuEntriesXOffset");
      MenuEntriesFirstY = EnsureGetMemberFloat("MenuEntriesFirstY");
      MenuEntriesYPadding = EnsureGetMemberFloat("MenuEntriesYPadding");
      MenuEntriesTargetWidth = EnsureGetMemberFloat("MenuEntriesTargetWidth");
      SkyInStartProgress = EnsureGetMemberFloat("SkyInStartProgress");
      SkyOutStartProgress = EnsureGetMemberFloat("SkyOutStartProgress");
      SkyMoveDurationIn = EnsureGetMemberFloat("SkyMoveDurationIn");
      SkyMoveDurationOut = EnsureGetMemberFloat("SkyMoveDurationOut");
      EntriesMoveDurationIn = EnsureGetMemberFloat("EntriesMoveDurationIn");
      EntriesMoveDurationOut = EnsureGetMemberFloat("EntriesMoveDurationOut");
      HighlightDurationIn = EnsureGetMemberFloat("HighlightDurationIn");
      HighlightDurationOut = EnsureGetMemberFloat("HighlightDurationOut");

      GetMemberSpriteArray(MenuEntriesSprites, MenuEntriesNum,
                           "MenuEntriesSprites");
      GetMemberSpriteArray(MenuEntriesHSprites, MenuEntriesNum,
                           "MenuEntriesHighlightedSprites");
    }
    Pop();
  }
}

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto