#include "systemmenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"
#include "../games/rne/systemmenu.h"
#include "../games/mo6tw/systemmenu.h"
#include "../games/chlcc/systemmenu.h"
#include "../games/mo8/systemmenu.h"
#include "../games/cclcc/systemmenu.h"
#include "../../ui/ui.h"
#include "../../log.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace SystemMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("SystemMenu")) {
    AssertIs(LUA_TTABLE);

    Type =
        SystemMenuType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

    MenuEntriesNum = EnsureGetMember<int>("MenuEntriesNum");
    MenuEntriesHNum = EnsureGetMember<int>("MenuEntriesHNum");
    MenuEntriesX = TryGetMember<float>("MenuEntriesX");
    MenuEntriesXOffset = TryGetMember<float>("MenuEntriesXOffset");
    MenuEntriesFirstY = TryGetMember<float>("MenuEntriesFirstY");
    MenuEntriesYPadding = TryGetMember<float>("MenuEntriesYPadding");

    if (MenuEntriesNum > 0) {
      GetMemberArray<Sprite>(MenuEntriesSprites, MenuEntriesNum,
                             "MenuEntriesSprites");
    }
    if (MenuEntriesHNum > 0) {
      GetMemberArray<Sprite>(MenuEntriesHSprites, MenuEntriesHNum,
                             "MenuEntriesHighlightedSprites");
    }

    TryGetMember<float>("FadeInDuration", FadeInDuration);
    TryGetMember<float>("FadeOutDuration", FadeOutDuration);

    if (Type == +SystemMenuType::RNE) {
      RNE::SystemMenu::Configure();
    } else if (Type == +SystemMenuType::MO6TW) {
      MO6TW::SystemMenu::Configure();
    } else if (Type == +SystemMenuType::CHLCC) {
      CHLCC::SystemMenu::Configure();
    } else if (Type == +SystemMenuType::MO8) {
      MO8::SystemMenu::Configure();
    } else if (Type == +SystemMenuType::CCLCC) {
      CCLCC::SystemMenu::Configure();
    }

    Pop();
  }
}

}  // namespace SystemMenu
}  // namespace Profile
}  // namespace Impacto