#include "titlemenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"
#include "../games/rne/titlemenu.h"
#include "../games/dash/titlemenu.h"
#include "../games/chlcc/titlemenu.h"
#include "../games/mo6tw/titlemenu.h"
#include "../games/mo8/titlemenu.h"
#include "../games/cc/titlemenu.h"
#include "../games/cclcc/titlemenu.h"
#include "../../log.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("TitleMenu")) {
    AssertIs(LUA_TTABLE);

    Type =
        TitleMenuType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

    MenuEntriesNum = EnsureGetMember<int>("MenuEntriesNum");
    if (MenuEntriesNum > 0) {
      GetMemberArray<Sprite>(MenuEntriesSprites, MenuEntriesNum,
                             "MenuEntriesSprites");
      GetMemberArray<Sprite>(MenuEntriesHSprites, MenuEntriesNum,
                             "MenuEntriesHighlightedSprites");
    }

    PressToStartSprite = EnsureGetMember<Sprite>("PressToStartSprite");

    PressToStartAnimDurationIn =
        EnsureGetMember<float>("PressToStartAnimDurationIn");
    PressToStartAnimDurationOut =
        EnsureGetMember<float>("PressToStartAnimDurationOut");

    PressToStartX = EnsureGetMember<float>("PressToStartX");
    PressToStartY = EnsureGetMember<float>("PressToStartY");

    if (Type == +TitleMenuType::RNE) {
      RNE::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::Dash) {
      Dash::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::CHLCC) {
      CHLCC::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::MO6TW) {
      MO6TW::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::MO8) {
      MO8::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::CC) {
      CC::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::CCLCC) {
      CCLCC::TitleMenu::Configure();
    }

    // if (Implementation != 0) {
    //  Implementation->PressToStartAnimation.DurationIn =
    //      PressToStartAnimDurationIn;
    //  Implementation->PressToStartAnimation.DurationOut =
    //      PressToStartAnimDurationOut;
    //  Implementation->PressToStartAnimation.LoopMode = ReverseDirection;
    //}

    Pop();
  }
}

}  // namespace TitleMenu
}  // namespace Profile
}  // namespace Impacto