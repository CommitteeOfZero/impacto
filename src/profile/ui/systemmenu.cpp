#include "systemmenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"
#include "../games/rne/systemmenu.h"
#include "../games/mo6tw/systemmenu.h"
#include "../../ui/ui.h"
#include "../../log.h"
#include "../../window.h"

namespace Impacto {
namespace Profile {
namespace SystemMenu {

using namespace Impacto::UI;

SystemMenuType Type = SystemMenuType::None;

Sprite MenuEntriesSprites[MenuEntriesNumMax];
Sprite MenuEntriesHSprites[MenuEntriesNumMax];

int MenuEntriesNum;
int MenuEntriesHNum;
float MenuEntriesX;
float MenuEntriesXOffset;
float MenuEntriesFirstY;
float MenuEntriesYPadding;
float FadeInDuration;
float FadeOutDuration;

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
  if (TryPushMember("SystemMenu")) {
    AssertIs(kObjectType);

    Type = SystemMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

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

    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

    if (Type == +SystemMenuType::RNE) {
      RNE::SystemMenu::Configure();
    } else if (Type == +SystemMenuType::MO6TW) {
      MO6TW::SystemMenu::Configure();
    }

    Pop();
  }
}

}  // namespace SystemMenu
}  // namespace Profile
}  // namespace Impacto